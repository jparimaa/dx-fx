cbuffer MatrixBuffer : register(b[0])
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix Jitter;
}

cbuffer PrevMatrixBuffer : register(b[1])
{
    matrix PrevWorld;
    matrix PrevView;
    matrix PrevProjection;
    matrix PrevJitter;
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
    float4 PrevPos : POSITIONT;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Pos = mul(output.Pos, Jitter);
    output.Normal = mul(input.Normal, World);
    output.Tex = input.Tex;
    return output;
}

Texture2D diffuseTex : register(t[0]);
SamplerState linearSampler : register(s[0]);

float4 PS(VS_OUTPUT input) :
    SV_Target
{
    return diffuseTex.Sample(linearSampler, input.Tex);
}