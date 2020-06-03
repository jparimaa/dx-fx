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

#include "sdf.hlsl"
#include "operations.hlsl"
#include "lighting.hlsl"

static const float c_epsilon = 0.0001;

cbuffer ConstantBuffer : register(b[0])
{
    float time;
    float3 padding;
    float4 cameraPos;
    float4 cameraDir;
    matrix cameraTransform;
    matrix sphere1Transform;
    matrix sphere2Transform;
    matrix sphere3Transform;
    matrix sphereBoxTransform;
}

float3 tr(in float3 p, in matrix m)
{
    return mul(float4(p, 1.0), m).xyz;
}

float3 trDir(in float3 p, in matrix m)
{
    return mul(float4(p, 0.0), m).xyz;
}

float sceneSDF(in float3 p)
{
    float a = sphereSDF(tr(p, sphere1Transform), 2.0);
    float b = sphereSDF(tr(p, sphere2Transform), 2.0);
    float c = sphereSDF(tr(p, sphere3Transform), 2.0);
    float sphere = sphereSDF(tr(p, sphereBoxTransform), 1.0);
    float box = boxSDF(tr(p, sphereBoxTransform), float3(1.0, 1.0, 1.0));
    float sphereBox = lerp(box, sphere, saturate(sin(time)));
    return conjunct(smoothConjunct(smoothConjunct(a, b, 1), c, 1), sphereBox);
}

float scene2SDF(in float3 p)
{
    return boxSDF(p, float3(0.5, 0.3, 0.1));
}

float4 raymarch(in float3 start, in float3 dir)
{
    static const int c_maxSteps = 1024;
    static const float c_maxDistance = 1000.0;

    float depth = 0.0;
    for (int i = 0; i < c_maxSteps; ++i)
    {
        float dist = sceneSDF(start + depth * dir);

        if (dist < c_epsilon)
        {
            return float4(start + depth * dir, 1.0);
        }

        depth += dist;

        if (depth >= c_maxDistance)
        {
            return float4(0.0, 0.0, 0.0, 0.0);
        }
    }
    return float4(0.0, 0.0, 0.0, 0.0);
}

float3 estimateNormal(in float3 p)
{
    return normalize(float3(
        sceneSDF(float3(p.x + c_epsilon, p.y, p.z)) - sceneSDF(float3(p.x - c_epsilon, p.y, p.z)),
        sceneSDF(float3(p.x, p.y + c_epsilon, p.z)) - sceneSDF(float3(p.x, p.y - c_epsilon, p.z)),
        sceneSDF(float3(p.x, p.y, p.z + c_epsilon)) - sceneSDF(float3(p.x, p.y, p.z - c_epsilon))));
}

float4 PS(VS_OUTPUT input) :
    SV_Target
{
    const float2 uvNormYUp = (input.Tex - 0.5) * 2.0 * float2(1.0, -1.0);
    const float3 uvDir = normalize(float3(uvNormYUp.x, uvNormYUp.y, 1.0));
    const float3 viewDir = trDir(uvDir, cameraTransform);
    const float3 dir = normalize(cameraDir.xyz + viewDir);
    float4 hit = raymarch(cameraPos.xyz, dir);
    if (hit.a > 0.0)
    {
        float3 n = estimateNormal(hit.xyz);
        float3 c = simpleLambert(n, float3(0.5, -0.5, 0.5), float3(0.2, 1.0, 0.7));
        return float4(c, 1.0);
    }

    return float4(0.0, 0.0, 0.2, 1.0);
}
