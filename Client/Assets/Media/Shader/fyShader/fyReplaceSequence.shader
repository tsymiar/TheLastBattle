Shader "fyShader/ReplaceSequence" 
{
		Properties 
		{	
			_Color("Color", Color) = (1,1,1,1)
			_MainTex ("Base (RGB)", 2D) = "white" {}			
			_SizeX ("列数", Float) = 4
			_SizeY ("行数", Float) = 4
			_Speed ("播放速度", Float) = 200
		}
    SubShader 
	{		
        Pass {		
			//渲染状态设置,必须放在顶部										
			Blend One Zero
			Cull Off
						
            CGPROGRAM	
			#pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"					
								
			//变量
			float4 _Color;
			float4x4 _TransMatrix;
			uniform fixed _SizeX;
			uniform fixed _SizeY;
			uniform fixed _Speed;
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
				int index = floor(_Time .x * _Speed);
				int indexY = index/_SizeX;
				int indexX = index - indexY*_SizeX;
				float2 seqUV = float2(i.texcoord0.x /_SizeX, i.texcoord0.y /_SizeY);
    
				seqUV.x += indexX/_SizeX;
				seqUV.y += indexY/_SizeY;
				return i.color0*_Color*tex2D(_MainTex, seqUV.xy);									
            }
            ENDCG
        }
    }
}