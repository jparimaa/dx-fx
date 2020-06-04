float sphereSDF(in float3 p, float radius)
{
    return length(p) - radius;
}

float boxSDF(in float3 p, in float3 b)
{
    float3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float coneSDF(in float3 p, in float2 c, float h)
{
    float q = length(p.xz);
    return max(dot(c.xy, float2(q, p.y)), -h - p.y);
}

float hexPrismSDF(in float3 p, in float2 h)
{
    const float3 k = float3(-0.8660254, 0.5, 0.57735);
    p = abs(p);
    p.xy -= 2.0 * min(dot(k.xy, p.xy), 0.0) * k.xy;
    float2 d = float2(length(p.xy - float2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.y - h.x),
                      p.z - h.y);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

// t.x = radius, t.y = thickness
float torusSDF(in float3 p, in float2 t)
{
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}
