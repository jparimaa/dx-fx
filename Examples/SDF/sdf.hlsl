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

static const float3 c_cameraPos = {0, 0, 0};
static const float c_epsilon = 0.0001;

cbuffer ConstantBuffer : register(b[0])
{
    float time;
    float3 padding;
}

float smoothUnion(float a, float b, float k = 32)
{
    float res = exp(-k * a) + exp(-k * b);
    return -log(max(0.0001, res)) / k;
}

float sphereSDF(in float3 p, in float3 center, in float radius)
{
    return distance(p, center) - radius;
}

float sceneSDF(in float3 p)
{
    float spherePos = 3.5 * sin(time); 
    float a = sphereSDF(p, float3(spherePos, 0.0, 10.0), 2.0);
    float b = sphereSDF(p, float3(-spherePos, 0.0, 10.0), 2.0);
    float c = sphereSDF(p, float3(0.0, spherePos, 10.0), 2.0);
    return smoothUnion(smoothUnion(a, b, 1), c, 1);
}

float3 estimateNormal(float3 p)
{
    return normalize(float3(
        sceneSDF(float3(p.x + c_epsilon, p.y, p.z)) - sceneSDF(float3(p.x - c_epsilon, p.y, p.z)),
        sceneSDF(float3(p.x, p.y + c_epsilon, p.z)) - sceneSDF(float3(p.x, p.y - c_epsilon, p.z)),
        sceneSDF(float3(p.x, p.y, p.z + c_epsilon)) - sceneSDF(float3(p.x, p.y, p.z - c_epsilon))));
}

float4 raymarch(in float3 viewDir)
{
    static const int c_maxSteps = 512;
    static const float c_maxDistance = 1000.0;

    float depth = 0.0;
    for (int i = 0; i < c_maxSteps; ++i)
    {
        float dist = sceneSDF(c_cameraPos + depth * viewDir);

        if (dist < c_epsilon)
        {
            return float4(c_cameraPos + depth * viewDir, 1.0);
        }

        depth += dist;

        if (depth >= c_maxDistance)
        {
            return float4(0.0, 0.0, 0.0, 0.0);
        }
    }
    return float4(0.0, 0.0, 0.0, 0.0);
}

float3 simpleLambert(float3 normal)
{
    const float3 lightDir = {0.5, -0.5, 0.5};
    const float3 lightColor = {0.2, 0.5, 0.7};

    float NdotL = max(dot(normal, -lightDir), 0.0);
    float3 c;
    c = lightColor * NdotL;
    return c;
}

float4 PS(VS_OUTPUT input) :
    SV_Target
{
    const float2 uvNormYUp = (input.Tex - 0.5) * 2.0 * float2(1.0, -1.0);
    const float3 viewDir = float3(uvNormYUp.x, uvNormYUp.y, 1.0) - c_cameraPos;
    float4 hit = raymarch(viewDir);
    if (hit.a > 0.0)
    {
        float3 n = estimateNormal(hit.xyz);
        float3 c = simpleLambert(n);
        return float4(c, 1.0);
    }

    return float4(uvNormYUp, sin(time), 1.0);
}
