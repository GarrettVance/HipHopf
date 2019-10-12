//         
// 
//                 ghv Instancing Vertex Shader 
// 
            
cbuffer WVP_ConstantBufferStruct : register(b0)
{
	matrix   model;
	matrix   view;
	matrix   projection;
    uint4    animator_count;
};



struct VertexShaderInput
{
	float3    pos       : POSITION;
	float2    texco     : TEXCOORD0; 
    float3    inst_pos  : TEXCOORD1; 
    uint      inst_id   : SV_InstanceID; 
};



struct PixelShaderInput
{
	float4     pos         : SV_POSITION;
	float2     texco       : TEXCOORD0;
    float4     anim_color  : COLOR; 
};



PixelShaderInput vs_main(VertexShaderInput input)
{
     PixelShaderInput output;
     
     float4 vertex_location = float4(input.pos, 1.0f);

     float4 tmp_anim_color = float4(0.f, 0.f, 0.f, 0.f); 
     

    //   Update the position of the vertices 
    //   based on the data for this particular instance.

    vertex_location.x += input.inst_pos.x;
    vertex_location.y += input.inst_pos.y;
    vertex_location.z += input.inst_pos.z;

    if(input.inst_id == animator_count.x)
    {
         // vertex_location.x *= 2.f;

         tmp_anim_color.x = 2.f;
    }


     // Transform the vertex position "vertex_location" into projected space.
     
     vertex_location = mul(vertex_location, model);
     vertex_location = mul(vertex_location, view);
     vertex_location = mul(vertex_location, projection);
     output.pos = vertex_location;
     
     output.texco = input.texco;

     output.anim_color = tmp_anim_color;
     
     return output;
}




