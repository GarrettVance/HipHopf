//               
//              
//                     ghv Instancing Pixel Shader  
//     


Texture2D ObjTexture;
SamplerState ObjSamplerState;



struct PixelShaderInput
{
	float4     pos         : SV_POSITION;
	float2     texco       : TEXCOORD0;
    float4     anim_color  : COLOR; 
};




float4       ps_main(PixelShaderInput      input) : SV_TARGET
{
     float4 v_rgba = ObjTexture.Sample( ObjSamplerState, input.texco );
     
     if(input.anim_color.x > 0.5)
     {
         v_rgba.r = 0.9f;
         v_rgba.g = 0.9f;
         v_rgba.b = 0.9f;
     }

     return float4(v_rgba.rgb, 1.0f);
}




