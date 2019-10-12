//         
// 
// 
    
          


cbuffer ModelAndNormalConstantBuffer : register(b0)   // Constant buffer for modelToWorld and normalToWorld;
{
    float4x4        modelToWorld;
    min16float4x4   normalToWorld;
    min16float4     colorFadeFactor;
};



cbuffer ViewProjectionConstantBuffer : register(b1)  // Matrices are stored in column-major order. 
{
    float4      cameraPosition;
    float4      lightPosition;
    float4x4    viewProjection; //  = ViewMatrix * PerspectiveProjectionMatrix; 
};


// cbuffer ModelViewProj : register(b0)
// {
// 	matrix   model;
// 	matrix   view;
// 	matrix   projection;
//  uint4    miscellaneous_uint4;
// };




// Vertex Shader Input Specification
// =============================================
struct VertexShaderInput
{
    min16float3     objectSpacePosition     : POSITION;
    min16float3     objectSpaceNormal       : NORMAL0;
    float3          guids                   : COLOR0;
	float2          texco                   : TEXCOORD0; 
};


// Vertex Shader Output Specification
// =============================================
struct VertexShaderOutput
{
    min16float4     screenPos       : SV_POSITION;
    min16float3     worldPos        : POSITION0;
    min16float3     worldNorm       : NORMAL0;
    float3          guids           : COLOR0;
    float2          texco           : TEXCOORD0;
};


VertexShaderOutput vs_main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    float4 pos = float4(input.objectSpacePosition, 1.0f);

    // Transform the vertex position into world space.
    pos = mul(pos, modelToWorld);

    // Store the world position.
    output.worldPos = (min16float3)pos;

    //  Correct for perspective and project the vertex position onto the screen.
    //  Note that the name "viewProjection" is intended 
    //  to convey "product of View matrix and Projection matrix". 
    pos = mul(pos, viewProjection);
    output.screenPos = (min16float4)pos;


    // Compute the normal.
    min16float4 normalVector = min16float4(input.objectSpaceNormal, min16float(0.f));
    output.worldNorm = normalize((min16float3)mul(normalVector, normalToWorld));

    output.guids = input.guids;

    output.texco = input.texco;

    return output;
}




