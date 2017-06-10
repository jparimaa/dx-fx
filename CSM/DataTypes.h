#pragma once

#include <fw/AssetManager.h>
#include <fw/Transformation.h>
#include <DirectXMath.h>
#include <d3d11.h>

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
	DirectX::XMVECTOR color;
};

struct DirectionalLightData
{
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR direction;
	DirectX::XMVECTOR color;
};