Shader "FX PACK 1/Energy Ball"
{
	Properties 
	{
_MainTex("Main_Texture", 2D) = "white" {}
_Color01("Main_Texture_Color", Color) = (1,1,1,1)
_Blend_Texture("Blend_Texture", 2D) = "white" {}
_Color02("Blend_Texture_Color", Color) = (1,1,1,1)
_Blend_Texture01("Blend_Texture01", 2D) = "black" {}
_Speed("Main_Texutre_Speed", Float) = 1
_Speed01("Blend_Texture_Speed", Float) = 1
_Speed02("Blend_Texture01_Speed", Float) = 1
_Fresnel_Value("Fresnel_Value", Range(0,3) ) = 0.5
_Lighten("Lighten", Float) = 1

	}
	
	SubShader 
	{
		Tags
		{
"Queue"="Transparent"
"IgnoreProjector"="False"
"RenderType"="Transparent"

		}

		
Cull Back
ZWrite On
ZTest LEqual
ColorMask RGBA
Fog{
}


		CGPROGRAM
#pragma surface surf BlinnPhongEditor  alpha decal:add vertex:vert
#pragma target 2.0


sampler2D _MainTex;
float4 _Color01;
sampler2D _Blend_Texture;
float4 _Color02;
sampler2D _Blend_Texture01;
float _Speed;
float _Speed01;
float _Speed02;
float _Fresnel_Value;
float _Lighten;

			struct EditorSurfaceOutput {
				half3 Albedo;
				half3 Normal;
				half3 Emission;
				half3 Gloss;
				half Specular;
				half Alpha;
				half4 Custom;
			};
			
			inline half4 LightingBlinnPhongEditor_PrePass (EditorSurfaceOutput s, half4 light)
			{
half3 spec = light.a * s.Gloss;
half4 c;
c.rgb = (s.Albedo * light.rgb + light.rgb * spec);
c.a = s.Alpha;
return c;

			}

			inline half4 LightingBlinnPhongEditor (EditorSurfaceOutput s, half3 lightDir, half3 viewDir, half atten)
			{
				half3 h = normalize (lightDir + viewDir);
				
				half diff = max (0, dot ( lightDir, s.Normal ));
				
				float nh = max (0, dot (s.Normal, h));
				float spec = pow (nh, s.Specular*128.0);
				
				half4 res;
				res.rgb = _LightColor0.rgb * diff;
				res.w = spec * Luminance (_LightColor0.rgb);
				res *= atten * 2.0;

				return LightingBlinnPhongEditor_PrePass( s, res );
			}
			
			struct Input {
				float3 viewDir;
float2 uv_MainTex;
float2 uv_Blend_Texture;
float2 uv_Blend_Texture01;
float4 color : COLOR;

			};

			void vert (inout appdata_full v, out Input o) {
float4 VertexOutputMaster0_0_NoInput = float4(0,0,0,0);
float4 VertexOutputMaster0_1_NoInput = float4(0,0,0,0);
float4 VertexOutputMaster0_2_NoInput = float4(0,0,0,0);
float4 VertexOutputMaster0_3_NoInput = float4(0,0,0,0);


			}
			

			void surf (Input IN, inout EditorSurfaceOutput o) {
				o.Normal = float3(0.0,0.0,1.0);
				o.Alpha = 1.0;
				o.Albedo = 0.0;
				o.Emission = 0.0;
				o.Gloss = 0.0;
				o.Specular = 0.0;
				o.Custom = 0.0;
				
float4 Fresnel0_1_NoInput = float4(0,0,1,1);
float4 Fresnel0=(1.0 - dot( normalize( float4( IN.viewDir.x, IN.viewDir.y,IN.viewDir.z,1.0 ).xyz), normalize( Fresnel0_1_NoInput.xyz ) )).xxxx;
float4 Pow0=pow(Fresnel0,_Fresnel_Value.xxxx);
float4 Multiply2=_Time * _Speed.xxxx;
float4 UV_Pan1=float4((IN.uv_MainTex.xyxy).x,(IN.uv_MainTex.xyxy).y + Multiply2.x,(IN.uv_MainTex.xyxy).z,(IN.uv_MainTex.xyxy).w);
float4 Tex2D0=tex2D(_MainTex,UV_Pan1.xy);
float4 Multiply5=_Color01 * Tex2D0;
float4 Multiply1=_Time * _Speed01.xxxx;
float4 UV_Pan0=float4((IN.uv_Blend_Texture.xyxy).x,(IN.uv_Blend_Texture.xyxy).y + Multiply1.x,(IN.uv_Blend_Texture.xyxy).z,(IN.uv_Blend_Texture.xyxy).w);
float4 Tex2D1=tex2D(_Blend_Texture,UV_Pan0.xy);
float4 Multiply6=_Color02 * Tex2D1;
float4 Add0=Multiply5 + Multiply6;
float4 Multiply0=Tex2D0 * Tex2D1;
float4 Multiply7=Add0 * Multiply0;
float4 Multiply3=Pow0 * Multiply7;
float4 Multiply10=_Time * _Speed02.xxxx;
float4 UV_Pan2=float4((IN.uv_Blend_Texture01.xyxy).x,(IN.uv_Blend_Texture01.xyxy).y + Multiply10.x,(IN.uv_Blend_Texture01.xyxy).z,(IN.uv_Blend_Texture01.xyxy).w);
float4 Tex2D2=tex2D(_Blend_Texture01,UV_Pan2.xy);
float4 Multiply8=Multiply3 * Tex2D2;
float4 Multiply9=Multiply8 * _Lighten.xxxx;
float4 Multiply4=Multiply9 * IN.color;
float4 Master0_0_NoInput = float4(0,0,0,0);
float4 Master0_1_NoInput = float4(0,0,1,1);
float4 Master0_3_NoInput = float4(0,0,0,0);
float4 Master0_4_NoInput = float4(0,0,0,0);
float4 Master0_5_NoInput = float4(1,1,1,1);
float4 Master0_7_NoInput = float4(0,0,0,0);
float4 Master0_6_NoInput = float4(1,1,1,1);
o.Emission = Multiply4;

				o.Normal = normalize(o.Normal);
			}
		ENDCG
	}
	Fallback "Diffuse"
}