Shader "Xffect/PP/radial_blur_mask" {
Properties {
 _MainTex ("Base (RGB)", 2D) = "white" {}
 _Mask ("Mask Texture (RG)", 2D) = "white" {}
}

SubShader
{
 Pass
 {
     ZTest Always Cull Off ZWrite Off
     Blend Off
     Fog { Mode off }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#pragma fragmentoption ARB_precision_hint_fastest 

#include "UnityCG.cginc"

uniform sampler2D _MainTex;
uniform sampler2D _Mask;
uniform half _SampleDist;
uniform half _SampleStrength;


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
    half2 dir = 0.5f - texCoord;

    
    fixed4 mainColor = tex2D(_MainTex, texCoord);
    fixed4 maskColor = tex2D(_Mask, texCoord);
    fixed mask = maskColor.r;
    
    fixed4 sum = mainColor;
    sum += tex2D(_MainTex, texCoord + dir*mask*_SampleDist*0.01);
    sum /= 2;
    
    return lerp(mainColor, sum, _SampleStrength);
}
ENDCG

 }
}

Fallback off

}