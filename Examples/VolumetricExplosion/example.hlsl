Texture3D<float> noiseVolumeTex : register(t0);
Texture2D<float4> gradientTex : register(t1);

SamplerState clampedSampler : register(s0);
SamplerState wrappedSampler : register(s1);

cbuffer ConstantBuffer : register(b0)
{
    matrix worldToViewMatrix;
    matrix worldToProjectionMatrix;
    matrix viewToWorldMatrix;

    float3 eyePositionWS;
    float noiseAmplitudeFactor;
    float3 eyeForwardWS;
    float noiseScale;

    float3 explosionPositionWS;
    float explosionRadiusWS;

    float3 noiseAnimationSpeed;
    float time;

    float edgeSoftness;
    float noiseFrequencyFactor;
    int primitiveIdx;
    float opacity;

    float displacementWS;
    float stepSizeWS;
    int maxNumSteps;
    float noiseInitialAmplitude;

    float2 uvScaleBias;
    float invMaxNoiseDisplacement;
    int numOctaves;

    float skinThickness;
    int numHullOctaves;
    int numHullSteps;
    float tessellationFactor;
};

void VS()
{
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants()
{
    HS_CONSTANT_DATA_OUTPUT Output;
    Output.TessFactor[0] = //
        Output.TessFactor[1] = //
        Output.TessFactor[2] = //
        Output.TessFactor[3] = //
        Output.InsideTessFactor[0] = //
        Output.InsideTessFactor[1] = tessellationFactor;
    return Output;
}

struct HS_OUTPUT
{
};

// clang-format off
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUTPUT HS()
// clang-format on
{
    HS_OUTPUT o = (HS_OUTPUT)0;
    return o;
}

float noise(float3 uvw)
{
    return noiseVolumeTex.SampleLevel(wrappedSampler, uvw, 0);
}

float fractalNoiseAtPositionWS(float3 posWS, uint numOctaves)
{
    const float3 animation = noiseAnimationSpeed * time;
    float3 uvw = posWS * noiseScale + animation;
    float amplitude = noiseInitialAmplitude;
    float noiseValue = 0.0f;

    for (uint i = 0; i < numOctaves; ++i)
    {
        noiseValue += abs(amplitude * noise(uvw));
        amplitude *= noiseAmplitudeFactor;
        uvw *= noiseFrequencyFactor;
    }

    return noiseValue * invMaxNoiseDisplacement;
}

float sphereSDF(float3 relativePosWS, float radiusWS)
{
    return length(relativePosWS) - radiusWS;
}

float sphereWithNoiseSDF(float3 posWS, float3 spherePositionWS, float radiusWS, float displacementWS, uint numOctaves, out float displacementOut)
{
    const float3 relativePosWS = posWS - spherePositionWS;
    const float signedDistanceToPrimitive = sphereSDF(relativePosWS, radiusWS);
    displacementOut = fractalNoiseAtPositionWS(posWS, numOctaves);
    return signedDistanceToPrimitive - displacementOut * displacementWS;
}

struct PS_INPUT
{
    float4 PosPS : SV_Position;
    noperspective float2 rayHitNearFar : RAYHIT;
    noperspective float3 rayDirectionWS : RAYDIR;
};

// clang-format off
[domain("quad")]
PS_INPUT DS(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
// clang-format on
{
    const float2 posClipSpace = UV.xy * 2.0f - 1.0f;
    const float2 posClipSpaceAbs = abs(posClipSpace.xy);
    const float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

    const float3 dir = normalize(float3(posClipSpace.xy, (maxLen - 1.0f)));

    // First get the front world space position of the hull.
    const float3 frontNormDir = dir;
    float3 frontPosWS = mul(float4(frontNormDir, 0), viewToWorldMatrix).xyz * explosionRadiusWS + explosionPositionWS;
    const float3 frontDirWS = normalize(frontPosWS);
    // Then perform the shrink wrapping step using sphere tracing.
    const float innerRadius = explosionRadiusWS - displacementWS;
    float dummy;
    for (int i = 0; i < numHullSteps; ++i)
    {
        float dist = sphereWithNoiseSDF(frontPosWS, explosionPositionWS.xyz, innerRadius, displacementWS, numHullOctaves, dummy);
        frontPosWS -= frontDirWS * dist;
    }
    frontPosWS += frontDirWS * skinThickness;
    const float4 frontPosVS = mul(float4(frontPosWS, 1), worldToViewMatrix);
    const float4 frontPosPS = mul(float4(frontPosWS, 1), worldToProjectionMatrix);

    // Even though our geometry only extends around the front of the explosion volume,
    // we can calculate the reverse side of the hull here aswell.
    // So repeat the process for the back faces.
    const float3 backNormDir = dir * float3(1, 1, -1);
    float3 backPosWS = mul(float4(backNormDir, 0), viewToWorldMatrix).xyz * explosionRadiusWS + explosionPositionWS;
    const float3 backDirWS = normalize(frontPosWS);
    for (int j = 0; j < numHullSteps; ++j)
    {
        float dist = sphereWithNoiseSDF(backPosWS, explosionPositionWS.xyz, innerRadius, displacementWS, numHullOctaves, dummy);
        backPosWS -= backDirWS * dist;
    }
    backPosWS += backDirWS * skinThickness;
    const float4 backPosVS = mul(float4(backPosWS, 1), worldToViewMatrix);

    const float3 rayDirectionWS = normalize(frontPosWS - eyePositionWS);

    PS_INPUT psInput;
    psInput.PosPS = frontPosPS;
    psInput.rayHitNearFar = float2(frontPosVS.z, backPosVS.z);
    psInput.rayDirectionWS = rayDirectionWS;
    return psInput;
}

float4 mapDisplacementToColor(const float displacement, const float2 uvScaleBias)
{
    float texcoord = saturate(mad(displacement, uvScaleBias.x, uvScaleBias.y));
    texcoord = 1 - (1 - texcoord) * (1 - texcoord); // Adjust
    float4 color = gradientTex.SampleLevel(clampedSampler, texcoord, 0);
    // Apply some more adjustments to the color post sample. Again, these should be made in the texture itself.
    color *= color;
    color.a = 0.5f;
    return color;
}

float4 sceneFunction(const float3 posWS, const float3 spherePositionWS, const float radiusWS, const float displacementWS, const float2 uvScaleBias)
{
    float displacementOut;
    const float distance = sphereWithNoiseSDF(posWS, spherePositionWS, radiusWS, displacementWS, numOctaves, displacementOut);
    float4 color = mapDisplacementToColor(displacementOut, uvScaleBias);
    // Rather than just using a binary in/out metric, we smooth the edge of the volume using a smoothstep so that we get soft edges.
    const float edgeFade = smoothstep(0.5f + edgeSoftness, 0.5f - edgeSoftness, distance);
    return color * float4(1..xxx, edgeFade);
}

float4 blend(const float4 src, const float4 dst)
{
    return mad(float4(dst.rgb, 1), mad(dst.a, -src.a, dst.a), src);
}

// clang-format off
float4 PS(PS_INPUT input) : SV_Target
// clang-format on
{
    const float3 rayDirectionWS = input.rayDirectionWS;
    const float near = input.rayHitNearFar.x;
    const float far = input.rayHitNearFar.y;

    float4 output;

    const float3 stepAmountWS = rayDirectionWS * stepSizeWS;
    const int numSteps = min(maxNumSteps, (far - near) / stepSizeWS);
    const float innerRadius = explosionRadiusWS - displacementWS;

    float3 posWS = mad(rayDirectionWS, near, eyePositionWS.xyz);

    int stepsTaken = 0;
    while (stepsTaken++ < numSteps && output.a < opacity)
    {
        float4 color = sceneFunction(posWS, explosionPositionWS.xyz, innerRadius, displacementWS, uvScaleBias);
        output = blend(output, color);
        posWS += stepAmountWS;
    }

    return output * float4(1..xxx, opacity);
}
