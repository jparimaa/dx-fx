#pragma once

#include <fw/Transformation.h>
#include <DirectXMath.h>
#include <d3d11.h>

struct VertexBuffers
{
	ID3D11Buffer* vertexBuffer = nullptr;
	UINT stride = 8 * sizeof(float);
	UINT offset = 0;
	ID3D11Buffer* indexBuffer = nullptr;
	unsigned int numIndices = 0;

	~VertexBuffers();
};

struct RenderData
{
	fw::Transformation transformation;
	VertexBuffers* vertexBuffers = nullptr;;
	ID3D11ShaderResourceView* textureView = nullptr;
};