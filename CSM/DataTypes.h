#pragma once

#include <fw/AssetManager.h>
#include <fw/Transformation.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <array>

struct MatrixData
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct RenderData
{
	ID3D11ShaderResourceView* textureView = nullptr;
	fw::AssetManager::VertexBuffer* vertexBuffer = nullptr;
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
};