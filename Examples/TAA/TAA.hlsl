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
    int enableMotionBuffer;
    float2 texelSize;
}

float4 PS(VS_OUTPUT input) :
    SV_Target
{
    // Simple neighbor min-max clamp to avoid ghosting. Could do fancier stuff here.
    float4 colorCenter = currentFrameTex.Sample(linearSampler, input.Tex);
    /*
    Same as below
    float4 colorUp = currentFrameTex.Sample(linearSampler, input.Tex + float2(0.0, texelSize.y));
    float4 colorRight = currentFrameTex.Sample(linearSampler, input.Tex + float2(texelSize.x, 0.0));
    float4 colorDown = currentFrameTex.Sample(linearSampler, input.Tex + float2(0.0, -texelSize.y));
    float4 colorLeft = currentFrameTex.Sample(linearSampler, input.Tex + float2(-texelSize.x, 0.0));
    */
    float4 colorUp = currentFrameTex.Sample(linearSampler, input.Tex, int2(0, 1));
    float4 colorRight = currentFrameTex.Sample(linearSampler, input.Tex, int2(1, 0));
    float4 colorDown = currentFrameTex.Sample(linearSampler, input.Tex, int2(0, -1));
    float4 colorLeft = currentFrameTex.Sample(linearSampler, input.Tex, int2(-1, 0));

    float4 minColor = min(colorCenter, min(colorUp, min(colorRight, min(colorDown, colorLeft))));
    float4 maxColor = max(colorCenter, max(colorUp, max(colorRight, max(colorDown, colorLeft))));

    float2 motion = motionTex.Sample(linearSampler, input.Tex);
    motion *= float(enableMotionBuffer);
    float4 historyColor = prevFrameTex.Sample(linearSampler, input.Tex + motion);
    float4 clampedHistoryColor = clamp(historyColor, minColor, maxColor);

    return lerp(clampedHistoryColor, colorCenter, blendRatio);
}