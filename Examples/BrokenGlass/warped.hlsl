Texture2D scene : register(t0);
Texture2D normalMap : register(t1);
SamplerState linearSampler : register(s0);

cbuffer ConstantBuffer : register(b[0])
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
	float2 Tex : TEXCOORD0;
};

PSData VS(VSData input)
{
	PSData output = (PSData)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Normal = mul(input.Normal, World);
	output.Tex = input.Tex;
	return output;
}

float4 PS(PSData input) : SV_Target
{
	float4 output = normalMap.Sample(linearSampler, input.Tex) + scene.Sample(linearSampler, input.Tex) * 0.1;
	output.a = 0.5;
	return output;
}