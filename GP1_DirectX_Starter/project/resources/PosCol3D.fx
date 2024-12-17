//---------------------------------------------------
// input/output structs
//---------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITIONT;
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Color : COLOR;
};


//---------------------------------------------------
// Vertex Shader
//---------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = float4(input.Position, 1.f);
    output.Color = input.Color;
    return output;
}


//---------------------------------------------------
// pixel shader
//---------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(input.Color, 1.f);
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