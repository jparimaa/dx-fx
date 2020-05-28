cbuffer MatrixBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Normal = mul(input.Normal, World);
    output.Tex = input.Tex;
    return output;
}

Texture2D diffuseTex : register(t[0]);
SamplerState linearSampler : register(s[0]);

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_OUTPUT PS(VS_OUTPUT input) :
    SV_Target
{
    PS_OUTPUT output;
    output.Color = diffuseTex.Sample(linearSampler, input.Tex);
    return output;
}