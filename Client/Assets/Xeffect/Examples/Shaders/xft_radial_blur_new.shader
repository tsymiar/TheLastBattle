Shader "Xffect/PP/radial_blur_new" {
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
#pragma target 3.0
#pragma vertex vert
#pragma fragment frag
#pragma fragmentoption ARB_precision_hint_fastest 

#include "UnityCG.cginc"

uniform sampler2D _MainTex;
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

half2 dir = i.uv - _Center;
fixed4 color = fixed4(0.0);
half scale;

for (int i = 0; i < 15; i++)
{
    scale = 1.0 + _SampleStrength * (i / 14.0);
    color += tex2D(_MainTex, dir * scale + _Center);
}

color /= 15.0;
return color;
}
ENDCG

 }
}

Fallback off

}