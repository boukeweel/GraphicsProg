//---------------------------------------------------
// Globals
//---------------------------------------------------
float4x4 g_WorldViewProjection : WorldViewProjection;
float4x4 g_MeshWorldMatrix : MeshWorldMatrix;
float3 g_LightDirection : LightDirection;
float3 g_CamaraOrigin : CamaraOrigin;

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
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Color : COLOR;
    float2 TextureUV : TEXCOORD;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
    
    float3 WorldPosition : WORLD;
    float3 ViewDirection : VIEWDIRECTION;
};


//---------------------------------------------------
// Vertex Shader
//---------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), g_MeshWorldMatrix);
    output.normal = normalize(mul(input.normal, (float3x3) g_MeshWorldMatrix));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) g_MeshWorldMatrix));
    
    output.WorldPosition = output.Position.xyz;
    output.ViewDirection = normalize(g_CamaraOrigin - output.WorldPosition);
    
    output.Position = mul(output.Position, g_WorldViewProjection);
    
    output.TextureUV = input.TextureUV;
    output.Color = input.Color;
    
    return output;
}


//---------------------------------------------------
// pixel shader
//---------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 sampleDiffuseColor = g_DiffuseMap.Sample(g_TextureSampler, input.TextureUV);
    
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