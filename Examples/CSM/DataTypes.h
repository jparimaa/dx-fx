#pragma once

#include <fw/Common.h>
#include <fw/Transformation.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <array>

static const int NUM_CASCADES = 3;

struct MatrixData
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct LightMatrixData
{
	DirectX::XMMATRIX viewProjection;
};

struct RenderData
{
	ID3D11ShaderResourceView* textureView = nullptr;
	fw::VertexBuffer* vertexBuffer = nullptr;
	fw::Transformation transformation;
};

struct DirectionalLight
{
	fw::Transformation transformation;
	std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
};

struct DirectionalLightData
{
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR direction;
	std::array<float, 4> color;
	std::array<float, NUM_CASCADES> cascadeLimits;
};