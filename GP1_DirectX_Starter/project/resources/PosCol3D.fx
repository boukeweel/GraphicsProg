//---------------------------------------------------
// Globals
//---------------------------------------------------
float4x4 g_WorldViewProjection : WorldViewProjection;
float4x4 g_MeshWorldMatrix : MeshWorldMatrix;

Texture2D g_DiffuseMap : DiffuseMap;

SamplerState g_TextureSampler : Sampler;

//---------------------------------------------------
// input/output structs
//---------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 TextureUV : TEXCOORD;

};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Color : COLOR;
    float2 TextureUV : TEXCOORD;

};


//---------------------------------------------------
// Vertex Shader
//---------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), g_MeshWorldMatrix);
    output.Color = input.Color;
    
    output.Position = mul(output.Position, g_WorldViewProjection);
    
    output.TextureUV = input.TextureUV;
    
    return output;
}


//---------------------------------------------------
// pixel shader
//---------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 sampleDiffuseColor = float4(1, 1, 1, 1);
    sampleDiffuseColor *= g_DiffuseMap.Sample(g_TextureSampler, input.TextureUV);
    return sampleDiffuseColor;
}

//---------------------------------------------------
// Technique
//---------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}