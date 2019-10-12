//               
//               
//  ghv 20191007:  Pixel Shader for lofting the Hopf Fibration. 
//     



cbuffer BufferInfoCB  : register(b0)
{
    int fiberColoringMode;
};



Texture2D       texturePureFail         : register(t0);
Texture2D       textureFerrero          : register(t1);
SamplerState    plainSampler;



cbuffer ModelAndNormalConstantBuffer : register(b0)   // Constant buffer for modelToWorld and normalToWorld;
{
    float4x4        modelToWorld;
    min16float4x4   normalToWorld;
    min16float3     colorFadeFactor;
};



cbuffer ViewProjectionConstantBuffer : register(b1)   // Matrices are stored in column-major order. 
{
    float4      cameraPosition;
    float4      lightPosition;
    float4x4    viewProjection; //  = ViewMatrix * PerspectiveProjectionMatrix; 
};








//  Pixel Shader Input Specification
//  =============================================
struct PixelShaderInput
{
    min16float4     screenPos       : SV_POSITION;
    min16float3     worldPos        : POSITION0;
    min16float3     worldNorm       : NORMAL0;
    float3          guids           : COLOR0;   //  guids.x is fiber guid; guids.y is torus guid;
    float2          texco           : TEXCOORD0;
};
    


//  The pixel shader applies simplified Blinn-Phong BRDF lighting.
//  =============================================================
min16float4 ps_main(PixelShaderInput input) : SV_TARGET
{
    min16float3 lightDiffuseColorValue = min16float3(1.f, 1.f, 1.f);

    min16float3 objectBaseColorValue = min16float3(0.3f, 0.2f, 0.7f);



    if (input.guids.y > 1.9)
    {
        objectBaseColorValue = min16float3(0.9f, 0.0f, 0.1f);
    }
    else if (input.guids.y > 0.9)
    {
        objectBaseColorValue = min16float3(0.3f, 0.2f, 0.8f);
        //  objectBaseColorValue = (min16float3)(textureFerrero.Sample(plainSampler, input.texco)).xyz;
    }
    else
    {
        objectBaseColorValue = min16float3(0.3f, 0.8f, 0.1f);
        //  objectBaseColorValue = (min16float3)(texturePureFail.Sample(plainSampler, input.texco)).xyz;
    }

    // N is the surface normal, which points directly away from the surface.
    min16float3 N = normalize(input.worldNorm);




    // L is the light incident vector, which is a normal that points from the surface to the light.
    min16float3 lightIncidentVectorNotNormalized = min16float3(lightPosition.xyz - input.worldPos);
    min16float  distanceFromSurfaceToLight = length(lightIncidentVectorNotNormalized);
    min16float  oneOverDistanceFromSurfaceToLight = min16float(1.f) / distanceFromSurfaceToLight;
    min16float3 L = normalize(lightIncidentVectorNotNormalized);







    // V is the camera incident vector, which is a normal that points from the surface to the camera.

    min16float3 V = normalize(min16float3(cameraPosition.xyz - input.worldPos));



    // H is a normalized vector that is halfway between L and V.

    min16float3 H = normalize(L + V);






    // We take the dot products of N with L and H.
    min16float nDotL = dot(N, L);
    min16float nDotH = dot(N, H);

    // The dot products should be clamped to 0 as a lower bound.
    min16float clampedNDotL = max(min16float(0.f), nDotL);
    min16float clampedNDotH = max(min16float(0.f), nDotH);

    // We can then use dot(N, L) to determine the diffuse lighting contribution.
    min16float3 diffuseColor = lightDiffuseColorValue * objectBaseColorValue * clampedNDotL;

    // The specular contribution is based on dot(N, H).
    const min16float  specularExponent = min16float(4.f);
    const min16float3 specularColorValue = min16float3(1.f, 1.f, 0.9f);


    const min16float experimentalNudge = min16float(0.5f); // Use 1; Actually should be zero;


    const min16float3 specularColor = 
        specularColorValue * 
        pow(experimentalNudge + clampedNDotH, specularExponent) * 
        oneOverDistanceFromSurfaceToLight;


    // Sum the ambient, diffuse, and specular contributions to determine the lighting value for the pixel.
    //      
    // surfaceLitColor = 20 percent objectBaseColor + 60 percent diffuseColor + 20 percent specularColor: 

    const min16float3 surfaceLitColor = 
        objectBaseColorValue * min16float(0.2f) + 
        diffuseColor * min16float(0.6f) +  
        specularColor * min16float(0.2f);
    

    return min16float4(surfaceLitColor, 1.f);
    // return min16float4(N, 1.f);
    // return min16float4(specularColor, 1.f);
}






