Texture2D scene : register(t0);
Texture2D normalMap : register(t1);
SamplerState linearSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VSData
{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct PSData
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;	
    float4 ScreenPos : TEXCOORD0;
	float2 Tex : TEXCOORD1;
};

PSData VS(VSData input)
{
	PSData output = (PSData)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Normal = input.Normal;
	output.ScreenPos = output.Pos;
	output.Tex = input.Tex;
	return output;
}


cbuffer ConstantBuffer : register(b1)
{
	matrix viewProj;
	float4 cameraPos;
}

// See http://www.thetenthplanet.de/archives/1180
float3 perturbNormal(PSData input)
{
	float3 q1 = ddx(input.Pos.xyz);
	float3 q2 = ddy(input.Pos.xyz);
	float2 st1 = ddx(input.Tex);
	float2 st2 = ddy(input.Tex);

	float3 N = normalize(input.Normal.xyz);
	float3 T = normalize(q1 * st2.x - q2 * st1.x);
	float3 B = -normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	float3 tangentNormal = normalMap.Sample(linearSampler, input.Tex).xyz * 2.0 - 1.0;
	float3 unnormalized = mul(tangentNormal, TBN);
	return normalize(unnormalized);
}

float4 PS(PSData input) : SV_Target
{
	float3 normal = perturbNormal(input);
	float3 cameraToPixel = normalize(input.Pos.xyz - cameraPos.xyz);
	float3 refracted = refract(normal, cameraToPixel, 1.0);
	float3 screenRefracted = normalize(mul(float4(refracted, 0.0), viewProj).xyz);
	float2 refractedUv = input.ScreenPos.xy + screenRefracted.xy;	
	float4 output = scene.Sample(linearSampler, refractedUv);
	//output.xyz = float3(input.ScreenPos.xy, 0.0);
	return output;
}