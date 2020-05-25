cbuffer MatrixBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix Jitter;
}

cbuffer PrevMatrixBuffer : register(b1)
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
    float4 CurrPos : POSITION0;
    float4 PrevPos : POSITION1;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.CurrPos = output.Pos;
    output.Pos = mul(output.Pos, Jitter);
    output.PrevPos = mul(input.Pos, PrevWorld);
    output.PrevPos = mul(output.PrevPos, PrevView);
    output.PrevPos = mul(output.PrevPos, PrevProjection);
    output.Normal = mul(input.Normal, World);
    output.Tex = input.Tex;
    return output;
}

Texture2D diffuseTex : register(t[0]);
SamplerState linearSampler : register(s[0]);

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    float2 Motion : SV_Target1;
};

PS_OUTPUT PS(VS_OUTPUT input) :
    SV_Target
{
    PS_OUTPUT output;
    output.Color = diffuseTex.Sample(linearSampler, input.Tex);

    float2 currentPos = input.CurrPos.xy / input.CurrPos.w;
    float2 prevPos = input.PrevPos.xy / input.PrevPos.w;
    output.Motion = (prevPos - currentPos) * float2(0.5, -0.5);
    return output;
}