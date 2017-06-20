cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
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
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 pos = float4(input.Pos, 1.0f);
	output.Pos = mul(pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);	
	return output;
}

// Write only depth value so no PS is needed
