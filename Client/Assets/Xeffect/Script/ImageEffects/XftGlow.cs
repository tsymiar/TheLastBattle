using UnityEngine;

//Just modified from unity's build-in Image Effect Glow.
[ExecuteInEditMode]
[RequireComponent (typeof(Camera))]
public class XftGlow : MonoBehaviour
{
	/// The brightness of the glow. Values larger than one give extra "boost".
	public float glowIntensity = 1.5f;
	
	/// Blur iterations - larger number means more blur.
	public int blurIterations = 3;
	
	/// Blur spread for each iteration. Lower values
	/// give better looking blur, but require more iterations to
	/// get large blurs. Value is usually between 0.5 and 1.0.
	public float blurSpread = 0.7f;
	
	/// Tint glow with this color. Alpha adds additional glow everywhere.
	public Color glowTint = new Color(1,1,1,1);
	
	protected XftEventComponent m_client = null;
	
	
	// --------------------------------------------------------
	// The final composition shader:
	//   adds (glow color * glow alpha * amount) to the original image.
	// In the combiner glow amount can be only in 0..1 range; we apply extra
	// amount during the blurring phase.
	
    public Shader compositeShader;
    Material m_CompositeMaterial = null;
	protected Material compositeMaterial {
		get {
			if (m_CompositeMaterial == null) {
                m_CompositeMaterial = new Material(compositeShader);
				m_CompositeMaterial.hideFlags = HideFlags.HideAndDontSave;
			}
			return m_CompositeMaterial;
		} 
	}
	
	
	// --------------------------------------------------------
	// The blur iteration shader.
	// Basically it just takes 4 texture samples and averages them.
	// By applying it repeatedly and spreading out sample locations
	// we get a Gaussian blur approximation.
	// The alpha value in _Color would normally be 0.25 (to average 4 samples),
	// however if we have glow amount larger than 1 then we increase this.

    public Shader blurShader;
    Material m_BlurMaterial = null;
	protected Material blurMaterial {
		get {
			if (m_BlurMaterial == null) {
                m_BlurMaterial = new Material(blurShader);
                m_BlurMaterial.hideFlags = HideFlags.HideAndDontSave;
			}
			return m_BlurMaterial;
		} 
	}
	
	
	// --------------------------------------------------------
	// The image downsample shaders for each brightness mode.
	// It is in external assets as it's quite complex and uses Cg.
	public Shader downsampleShader;
	Material m_DownsampleMaterial = null;
	protected Material downsampleMaterial {
		get {
			if (m_DownsampleMaterial == null) {
				m_DownsampleMaterial = new Material( downsampleShader );
				m_DownsampleMaterial.hideFlags = HideFlags.HideAndDontSave;
			}
			return m_DownsampleMaterial;
		} 
	}
	
	
	// --------------------------------------------------------
	//  finally, the actual code
	
	/*protected void OnDisable()
    {
		if( m_CompositeMaterial ) {
			DestroyImmediate( m_CompositeMaterial );
		}
		if( m_BlurMaterial ) {
			DestroyImmediate( m_BlurMaterial );
		}
		if( m_DownsampleMaterial )
			DestroyImmediate( m_DownsampleMaterial );
	}*/
	
	void Awake()
	{
		this.enabled = false;
	}
	
	public void Init(XftEventComponent client)
	{
		
		m_client = client;
		if (m_DownsampleMaterial == null)
		{
			downsampleShader = client.GlowDownSampleShader;
			m_DownsampleMaterial = new Material( downsampleShader );
			m_DownsampleMaterial.hideFlags = HideFlags.HideAndDontSave;
		}

		if (m_CompositeMaterial == null)
		{
			compositeShader = client.GlowCompositeShader;
			m_CompositeMaterial = new Material(compositeShader);
			m_CompositeMaterial.hideFlags = HideFlags.HideAndDontSave;
		}

		if (m_BlurMaterial == null)
		{
			blurShader = client.GlowBlurShader;
			m_BlurMaterial = new Material(blurShader);
			m_BlurMaterial.hideFlags = HideFlags.HideAndDontSave;
		}
		SetClientParam();
	}
	
	void SetClientParam()
	{
		glowIntensity = m_client.GlowIntensity;
		blurIterations = m_client.GlowBlurIterations;
		blurSpread = m_client.GlowBlurSpread;
		glowTint = m_client.GlowColorStart;
	}
	
	public bool CheckSupport()
	{
		bool ret = true;
		if (!SystemInfo.supportsImageEffects)
		{
			ret = false;
		}
		
		// Disable the effect if no downsample shader is setup
		if( downsampleShader == null )
		{
			Debug.Log ("No downsample shader assigned! Disabling glow.");
			ret = false;
		}
		// Disable if any of the shaders can't run on the users graphics card
		else
		{		
			if( !blurMaterial.shader.isSupported )
				ret = false;
			if( !compositeMaterial.shader.isSupported )
				ret = false;
			if( !downsampleMaterial.shader.isSupported )
				ret = false;
		}
		
		return ret;
	}
	
	
	// Performs one blur iteration.
	public void FourTapCone (RenderTexture source, RenderTexture dest, int iteration)
	{
		float off = 0.5f + iteration*blurSpread;
		Graphics.BlitMultiTap (source, dest, blurMaterial,
            new Vector2( off, off),
			new Vector2(-off, off),
            new Vector2( off,-off),
            new Vector2(-off,-off)
		);
	}
	
	// Downsamples the texture to a quarter resolution.
	private void DownSample4x (RenderTexture source, RenderTexture dest)
	{
		downsampleMaterial.color = new Color( glowTint.r, glowTint.g, glowTint.b, glowTint.a/4.0f );
		Graphics.Blit (source, dest, downsampleMaterial);
	}
	
	// Called by the camera to apply the image effect
	void OnRenderImage (RenderTexture source, RenderTexture destination)
	{
		// Clamp parameters to sane values
		glowIntensity = Mathf.Clamp( glowIntensity, 0.0f, 10.0f );
		blurIterations = Mathf.Clamp( blurIterations, 0, 30 );
		blurSpread = Mathf.Clamp( blurSpread, 0.5f, 1.0f );
		
		RenderTexture buffer = RenderTexture.GetTemporary(source.width/4, source.height/4, 0);
		RenderTexture buffer2 = RenderTexture.GetTemporary(source.width/4, source.height/4, 0);
		
		// Copy source to the 4x4 smaller texture.
		DownSample4x (source, buffer);
		
		// Blur the small texture
		float extraBlurBoost = Mathf.Clamp01( (glowIntensity - 1.0f) / 4.0f );
		blurMaterial.color = new Color( 1F, 1F, 1F, 0.25f + extraBlurBoost );
		
		bool oddEven = true;
		for(int i = 0; i < blurIterations; i++)
		{
			if( oddEven )
				FourTapCone (buffer, buffer2, i);
			else
				FourTapCone (buffer2, buffer, i);
			oddEven = !oddEven;
		}
		Graphics.Blit(source,destination);
				
		if( oddEven )
			BlitGlow(buffer, destination);
		else
			BlitGlow(buffer2, destination);
		
		RenderTexture.ReleaseTemporary(buffer);
		RenderTexture.ReleaseTemporary(buffer2);
	}
	
	public void BlitGlow( RenderTexture source, RenderTexture dest )
	{
		compositeMaterial.color = new Color(1F, 1F, 1F, Mathf.Clamp01(glowIntensity));
		Graphics.Blit (source, dest, compositeMaterial);
	}	
}
