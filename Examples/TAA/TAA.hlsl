struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(uint id
             : SV_VertexID)
{
    VS_OUTPUT output;
    output.Tex = float2((id << 1) & 2, id & 2);
    output.Pos = float4(output.Tex * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

Texture2D currentFrameTex : register(t[0]);
Texture2D prevFrameTex : register(t[1]);
Texture2D motionTex : register(t[2]);
SamplerState linearSampler : register(s[0]);

cbuffer TAAParameters : register(b[0])
{
    float blendRatio;
    float3 padding;
}

float4 PS(VS_OUTPUT input) :
    SV_Target
{
    float4 currentFrameColor = currentFrameTex.Sample(linearSampler, input.Tex);
    float2 motion = motionTex.Sample(linearSampler, input.Tex);
    float4 prevFrameColor = prevFrameTex.Sample(linearSampler, input.Tex + motion);
    return lerp(prevFrameColor, currentFrameColor, blendRatio);
}