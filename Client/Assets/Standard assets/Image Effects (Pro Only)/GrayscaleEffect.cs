using UnityEngine;

[ExecuteInEditMode]
[AddComponentMenu("Image Effects/Color Adjustments/Grayscale")]
public class GrayscaleEffect : ImageEffectBase {
	public Texture  textureRamp;
	public float    rampOffset;
    public float    texFactor;

	// Called by camera to apply image effect
	void OnRenderImage (RenderTexture source, RenderTexture destination) {
		material.SetTexture("_RampTex", textureRamp);
		material.SetFloat("_RampOffset", rampOffset);
        material.SetFloat("_WeightFactor", texFactor);
		Graphics.Blit (source, destination, material);
	}
}