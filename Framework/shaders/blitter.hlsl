Texture2D source : register(t0);
SamplerState linearSampler : register(s0);

struct VSData
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct PSData
{
	float4 Pos : SV_POSITION;	
	float2 Tex : TEXCOORD0;
};

PSData VS(VSData input)
{
	PSData output;
	output.Pos = input.Pos;
	output.Tex = input.Tex;
	return output;
}

float4 PS(PSData input) : SV_Target
{
	return source.Sample(linearSampler, input.Tex);
}