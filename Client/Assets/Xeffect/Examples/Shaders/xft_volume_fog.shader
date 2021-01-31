//http://forum.unity3d.com/threads/142245-Xffect-Editor-Pro-powerful-tool-to-create-amazing-effects!
Shader "Xffect/volume_fog" {
Properties {
	_FogColor ("Fog Color", Color) = (1,1,1,1)
}

Category {
	Tags { "Queue"="Transparent+99" "IgnoreProjector"="True" "RenderType"="Transparent" }
	Blend SrcAlpha OneMinusSrcAlpha
	Cull Off Lighting Off ZWrite Off
	ZTest Always

SubShader {
    Pass {

CGPROGRAM
#pragma target 3.0
#pragma vertex vert
#pragma fragment frag
#include "UnityCG.cginc"

inline float CalcVolumeFogIntensity(float3 sphereCenter, float sphereRadius, float3 cameraPosition, float3 viewDirection, float backDepth, float maxDistance, float density)
{
	float3 local = cameraPosition - sphereCenter;
	float  fA      = dot(viewDirection, viewDirection);
	float  fB      = 2 * dot(viewDirection, local);
	float  fC      = dot(local, local) - sphereRadius * sphereRadius;
	float  fD      = fB * fB - 4 * fA * fC;
	if ( fD < 0.0f )
		return 0;
  
  float recpTwoA = 0.5 / fA;
  
  float dist;

  if (fD == 0.0f)
  {
    dist = backDepth;
  }
  else
  {
    float DSqrt = sqrt(fD);
    dist = (-fB - DSqrt) * recpTwoA;
  }

  dist = min(dist, maxDistance);
  backDepth = min(backDepth, maxDistance);
  
  float sample = dist;
  float fog = 0;
  float step_distance = ( backDepth - dist ) / 10;
  for ( int seg = 0; seg < 10; seg++ )
  {
    float3 position = cameraPosition + viewDirection * sample;
    fog += 1 - saturate( length( sphereCenter - position ) / sphereRadius );
    sample += step_distance;
  }
  fog /= 10;
  fog  = saturate( fog * density );
  return fog;
}

fixed4 _FogColor;
sampler2D _CameraDepthTexture;
uniform float4 FogParam;

struct v2f {
    float4 pos : SV_POSITION;
	float3 view : TEXCOORD0;
	float4 projPos : TEXCOORD1;
};


v2f vert (appdata_base v)
{
    v2f o;
	float4 wPos = mul (_Object2World, v.vertex);
    o.pos = mul (UNITY_MATRIX_MVP, v.vertex);
	o.view = wPos.xyz - _WorldSpaceCameraPos;
	o.projPos = ComputeScreenPos (o.pos);

	// move projected z to near plane if point is behind near plane
	float inFrontOf = ( o.pos.z / o.pos.w ) > 0;
	o.pos.z *= inFrontOf; 
    return o;
}

half4 frag (v2f i) : COLOR
{
	half4 color = half4(1,1,1,1);
	float depth = LinearEyeDepth (UNITY_SAMPLE_DEPTH(tex2Dproj(_CameraDepthTexture, UNITY_PROJ_COORD(i.projPos))));
	float backDist = length(i.view);
	float3 viewDir = normalize(i.view);
	float fog = CalcVolumeFogIntensity(FogParam.xyz, FogParam.w, _WorldSpaceCameraPos, viewDir, backDist, depth,_FogColor.a);
	
	color.rgb = _FogColor.rgb;
	color.a = fog;
    return color;
}
ENDCG

		}
	}
}
Fallback "VertexLit"
} 
