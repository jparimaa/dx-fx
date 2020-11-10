Texture2D diffuseTex : register(t0);
SamplerState linearSampler : register(s0);

cbuffer ConstantBuffer : register(b[0])
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    return output;
}

float4 PS(VS_OUTPUT input) :
    SV_Target
{
    return float4(0.0, 1.0, 1.0, 1.0);
}