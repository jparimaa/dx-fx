Texture2D diffuseTex : register(t0);
Texture2D depthmapTex : register(t1);
SamplerState linearSampler : register(s0);
SamplerState shadowSampler : register(s1);

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
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 ScreenPos : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 pos = float4(input.Pos, 1.0f);
	output.Pos = mul(pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.ScreenPos = output.Pos;
	// No support for non-uniform scaling
	output.Normal = mul(input.Normal, (float3x3)World);
	output.Tex = input.Tex;	
	return output;
}

cbuffer LightBuffer : register(b1)
{
	float4 LightPosition : POSITION;
	float4 LightDirection;
	float4 LightColor : COLOR;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float2 screenPos = ((input.ScreenPos.xy / input.ScreenPos.w) + (1,1)) / 2.0;
	screenPos.y = 1.0 - screenPos.y;
	float depth = depthmapTex.Sample(linearSampler, screenPos).r;
	depth = pow(depth, 8);
	//return float4(depth, depth, depth, 1.0);
	
	float diff = max(dot(normalize(input.Normal.xyz), float3(normalize(-LightDirection.xyz))), 0.0);
	float3 texColor = diffuseTex.Sample(linearSampler, input.Tex).xyz;
	float3 outColor = texColor * diff * LightColor.xyz * LightColor.w;
	
	return float4(outColor, 1.0);
}