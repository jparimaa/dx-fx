static const int NUM_CASCADES = 3;

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer LightMatrixBuffer : register(b1)
{
	matrix LightViewProjection[NUM_CASCADES];
}

struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 ScreenPos : TEXCOORD1;
	float4 Pos_Light[NUM_CASCADES] : POSITIONS;
	float Depth : DEPTH;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 pos_World = mul(float4(input.Pos, 1.0f), World);
	output.Pos = mul(pos_World, View);
	output.Depth = output.Pos.z;
	output.Pos = mul(output.Pos, Projection);
	// No support for non-uniform scaling
	output.Normal = mul(input.Normal, (float3x3)World);
	output.Tex = input.Tex;
	output.ScreenPos = output.Pos;
	
	for (int i = 0; i < NUM_CASCADES; ++i) {
		output.Pos_Light[i] = mul(pos_World, LightViewProjection[i]);
	}	
	return output;
}

cbuffer LightBuffer : register(b0)
{
	float4 LightPosition : POSITION;
	float4 LightDirection : DIRECTION;
	float4 LightColor : COLOR;
	float4 CascadeLimits[NUM_CASCADES / 4 + 1] : LIMITS;
}

Texture2D diffuseTex : register(t0);
Texture2D depthmapTex : register(t1);
SamplerState linearSampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	int cascadeIndex = NUM_CASCADES - 1;
    for (int i = 0; i < NUM_CASCADES; ++i) {
		if (input.Depth < CascadeLimits[i / 4][i % 4]) {
			cascadeIndex = i;
			break;
		}
	}

    float3 NDC = input.Pos_Light[cascadeIndex].xyz;// / input.Pos_Light.w; // w == 1
	float projectedDepth = NDC.z;
	float3 projectedCoordinates = NDC * 0.5 + 0.5;		
	float2 shadowmapUV;
	float offset = 1.0f / NUM_CASCADES;	
	shadowmapUV.x = cascadeIndex * offset + projectedCoordinates.x * offset;
	shadowmapUV.y = 1.0 - projectedCoordinates.y;
	float shadowmapDepth = depthmapTex.Sample(linearSampler, shadowmapUV).r;
	float bias = 0.002;		
	float shadow = projectedDepth - bias > shadowmapDepth ? 0.0 : 1.0;
		
	float diff = max(dot(normalize(input.Normal.xyz), float3(normalize(-LightDirection.xyz))), 0.0);
	float3 texColor = diffuseTex.Sample(linearSampler, input.Tex).xyz;
	float3 outColor = texColor * diff * LightColor.xyz * LightColor.w * shadow;

	// Debug purposes
	int channel = cascadeIndex % 3;
	if (channel == 0) outColor.r += 0.3f;
	if (channel == 1) outColor.g += 0.3f;
	if (channel == 2) outColor.b += 0.3f;

	return float4(outColor, 1.0);
}