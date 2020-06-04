float3 simpleLambert(in float3 normal, in float3 lightDir, in float3 lightColor)
{
    float NdotL = max(dot(normal, -lightDir), 0.0);
    return lightColor * NdotL;
}
