

// A constant buffer that stores the three basic column-major matrices for composing geometry.



cbuffer conbuf_ViewProjectionTransform : register(b0)
{
	matrix      view_transform;
	matrix      projection_transform;
};



cbuffer conbuf_ModelTransform : register(b1)
{
	matrix      model_transform;
};


struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};


struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};


PixelShaderInput   vs_main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos_model_space = float4(input.pos, 1.0f);

	// Transform the vertex position 
    // from Model Space into projected space (aka Clip Space): 

	float4 pos_world_space = mul(pos_model_space, model_transform);

	float4 pos_eye_space = mul(pos_world_space, view_transform);

	float4 pos_clip_space = mul(pos_eye_space, projection_transform);

	output.pos = pos_clip_space;
	output.color = input.color;
	return output;
}
