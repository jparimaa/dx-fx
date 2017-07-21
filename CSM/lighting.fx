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
	float4 Pos_Light : POSITIONT;
	int CascadeIndex : INDEX;
};

// Temporal
int getCascadeIndex(float z)
{
	if (z < 5.0) return 0;
	if (z < 15.0) return 1;
	return 2;
}

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 pos = float4(input.Pos, 1.0f);
	output.Pos = mul(pos, World);
	output.Pos = mul(output.Pos, View);
	float z = output.Pos.z;
	output.Pos = mul(output.Pos, Projection);
	// No support for non-uniform scaling
	output.Normal = mul(input.Normal, (float3x3)World);
	output.Tex = input.Tex;
	output.ScreenPos = output.Pos;
	output.Pos_Light = mul(pos, World);
	output.CascadeIndex = getCascadeIndex(z);
	output.Pos_Light = mul(output.Pos_Light, LightViewProjection[output.CascadeIndex]);
	return output;
}

cbuffer LightBuffer : register(b0)
{
	float4 LightPosition : POSITION;
	float4 LightDirection;
	float4 LightColor : COLOR;
}

Texture2D diffuseTex : register(t0);
Texture2D depthmapTex : register(t1);
SamplerState linearSampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 NDC = input.Pos_Light.xyz;// / input.Pos_Light.w; // w == 1
	float projectedDepth = NDC.z;
	float3 projectedCoordinates = NDC * 0.5 + 0.5;		
	float2 shadowmapUV;
	float offset = 1.0f / NUM_CASCADES;
	shadowmapUV.x = input.CascadeIndex * offset + projectedCoordinates.x * offset;
	shadowmapUV.y = 1.0 - projectedCoordinates.y;
	float shadowmapDepth = depthmapTex.Sample(linearSampler, shadowmapUV).r;
	float bias = 0.002;		
	float shadow = projectedDepth - bias > shadowmapDepth ? 0.0 : 1.0;
		
	float diff = max(dot(normalize(input.Normal.xyz), float3(normalize(-LightDirection.xyz))), 0.0);
	float3 texColor = diffuseTex.Sample(linearSampler, input.Tex).xyz;
	float3 outColor = texColor * diff * LightColor.xyz * LightColor.w * shadow;

	return float4(outColor, 1.0);
}