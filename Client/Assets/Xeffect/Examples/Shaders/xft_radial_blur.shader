Shader "Xffect/PP/radial_blur" {
Properties {
	_MainTex ("Base (RGB)", 2D) = "white" {}
}

SubShader
{
	Pass
	{
		ZTest Always Cull Off ZWrite Off
		Fog { Mode off }

CGPROGRAM
// Upgrade NOTE: excluded shader from Xbox360 because it uses wrong array syntax (type[size] name)
#pragma exclude_renderers xbox360
#pragma target 3.0
#pragma vertex vert
#pragma fragment frag
#pragma fragmentoption ARB_precision_hint_fastest 

#include "UnityCG.cginc"

uniform sampler2D _MainTex;
uniform half _SampleDist;
uniform half _SampleStrength;
uniform fixed4 _Center;

struct v2f {
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

v2f vert (appdata_img v)
{
	v2f o;
	o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
	o.uv = v.texcoord.xy;
	return o;
}

fixed4 frag (v2f i) : COLOR
{
	half2 texCoord = i.uv;
	half2 dir = _Center.xy - texCoord;
	half dist = length(dir);
	dir /= dist;
	
	fixed4 color = tex2D(_MainTex, texCoord);
	fixed4 sum = color;

	fixed samples[10] = fixed[10](-0.08,-0.05,-0.03,-0.02,-0.01,0.01,0.02,0.03,0.05,0.08);

	for (int i = 0; i < 10; ++i)  
	{  
		sum += tex2D(_MainTex, texCoord + dir*samples[i]*_SampleDist);
	}

	sum /= 11.0;
	fixed t = saturate(dist * _SampleStrength);
	return lerp(sum,color,t);
}
ENDCG

	}
}

Fallback off

}