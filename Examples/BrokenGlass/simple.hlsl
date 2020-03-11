Texture2D diffuseTex : register(t0);
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

struct ShaderOut
{
	float4 t1: SV_Target0;
	float4 t2: SV_Target1;
};

ShaderOut PS(PSData input)
{
	float4 output = diffuseTex.Sample(linearSampler, input.Tex);
	ShaderOut shaderOut;
	shaderOut.t1 = output;
	shaderOut.t2 = output;
	return shaderOut;
}