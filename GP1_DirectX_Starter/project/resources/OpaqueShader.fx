//---------------------------------------------------
// Globals
//---------------------------------------------------
float4x4 g_WorldViewProjection : WorldViewProjection;
float4x4 g_MeshWorldMatrix : MeshWorldMatrix;
float3 g_LightDirection : LightDirection;
float3 g_CamaraOrigin : CamaraOrigin;
bool g_UseNormalMap : UseNormalMap;

Texture2D g_DiffuseMap : DiffuseMap;
Texture2D g_NormalMap : NormalMap;
Texture2D g_SpecularMap : SpecularMap;
Texture2D g_GlossMap : GlossMap;

SamplerState g_TextureSampler : Sampler;

float g_LightIntensity : LightIntensity;
float g_PhongExponent : PhongExponent;
float g_PhongSpecular : PhongSpecular;

float3 g_AmbientColor : AmbientColor;


BlendState g_BlendState{};

DepthStencilState g_DepthStencilState{};

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
    float4 SampleDiffuseColor = float4(1, 1, 1, 1);
    float4 ambientcolor = float4(g_AmbientColor,1);
    float3 SampledNormal = input.normal;
    float LightIntesity = g_LightIntensity;
    float PhongExponent = g_PhongExponent;
    float PhongSpecular = g_PhongSpecular;
    
    SampleDiffuseColor *= g_DiffuseMap.Sample(g_TextureSampler, input.TextureUV);
    PhongSpecular *= g_SpecularMap.Sample(g_TextureSampler, input.TextureUV);
    PhongExponent *= g_GlossMap.Sample(g_TextureSampler, input.TextureUV);
    
    //Normals
    float3 biNormal = cross(input.normal, input.Tangent);
    float3x3 tangentSpaceAxis = float3x3(input.Tangent, biNormal, input.normal);
    
    float4 sampleNormalColor = g_NormalMap.Sample(g_TextureSampler, input.TextureUV);
    float3 sampledNormalMapped = float3(2.0f * sampleNormalColor.r - 1.0f, 2.0f * sampleNormalColor.g - 1.0f, 2.0f * sampleNormalColor.b - 1.0f);
    
    SampledNormal = normalize(mul(sampledNormalMapped, tangentSpaceAxis));
    
    //labertdiffuse
    float4 lambertdiffuse = SampleDiffuseColor * LightIntesity / 3.14f;
    
    //cosine law
    float observerdArea = max(0.0f, dot(SampledNormal, -g_LightDirection));
    
    //phong
    float3 reflected = reflect(g_LightDirection, SampledNormal);
    float cosAlpha = max(0.0f, dot(reflected, input.ViewDirection));
    float4 phongColor = float4(1, 1, 1, 1) * PhongSpecular * pow(cosAlpha, PhongExponent);
    
    return (phongColor + lambertdiffuse) * observerdArea + ambientcolor;
}

//---------------------------------------------------
// Technique
//---------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetDepthStencilState(g_DepthStencilState, 0);
        SetBlendState(g_BlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}