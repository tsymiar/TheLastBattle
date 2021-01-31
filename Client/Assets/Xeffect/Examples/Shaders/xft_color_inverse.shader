Shader "Xffect/PP/color_inverse" {
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
uniform fixed _Strength;


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
    fixed4 mainColor = tex2D(_MainTex, i.uv);
    fixed4 inverse = (1,1,1,1) - mainColor;
    
    return lerp(mainColor, inverse, _Strength);
}
ENDCG

 }
}

Fallback off

}