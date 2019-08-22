Shader "fyShader/Additive" 
{
		Properties 
		{			
			_Color("Color", Color) = (1,1,1,1)
			_MainTex ("Base (RGB)", 2D) = "white" {}									
		}
    SubShader 
	{		
        Pass {		
			//渲染状态设置,必须放在顶部										
			Blend One One
			Cull Off
						
            CGPROGRAM	
			#pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"					
								
			//变量
			float4 _Color;
			float4x4 _TransMatrix;
			uniform sampler2D _MainTex;
										
            struct vertexInput {
                float4 vertex : POSITION;
                float4 texcoord0 : TEXCOORD0;
				float4 color0:COLOR0;
            };

            struct fragmentInput{
                float4 position : POSITION;
                float4 texcoord0 : TEXCOORD0;
				float4 color0:COLOR0;
            };

			//顶点Shader
            fragmentInput vert(vertexInput i)
			{
                fragmentInput o;
                o.position = mul (UNITY_MATRIX_MVP, i.vertex);			
                o.texcoord0 = i.texcoord0;
				o.color0 = i.color0;
                return o;
            }
            			
			//像素Shader
			float4 frag(fragmentInput i) : COLOR 
			{   
				float4 transUV = mul(_TransMatrix,i.texcoord0);
				float4 color = i.color0*_Color*tex2D(_MainTex, transUV.xy);	
				return color;
            }
            ENDCG
        }
    }
}