float sphereSDF(in float3 p, float radius)
{
    return length(p) - radius;
}

// t.x = radius, t.y = thickness
float torusSDF(in float3 p, in float2 t)
{
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}