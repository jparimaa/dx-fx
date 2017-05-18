cbuffer ConstantBuffer : register(b[0])
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
};

VS_OUTPUT VS(float4 Pos : POSITION, float4 Normal : NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Normal = mul(Normal, World);
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(input.Normal.x, input.Normal.y, input.Normal.z, 1.0f);
}