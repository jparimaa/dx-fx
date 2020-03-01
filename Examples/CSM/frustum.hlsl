cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_INPUT
{
	float3 Pos : POSITION;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 pos = float4(input.Pos, 1.0f);
	output.Pos = mul(pos, View);
	output.Pos = mul(output.Pos, Projection);	
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return float4(1.0, 1.0, 0.0, 1.0);
}