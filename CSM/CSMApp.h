#pragma once

#include "RenderData.h"
#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Common.h>
#include <fw/Transformation.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <d3d11.h>
#include <memory>

class CSMApp : public fw::Application
{
public:
	CSMApp();
	virtual ~CSMApp();

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	struct MatrixData
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	fw::Camera camera;
	fw::CameraController cameraController;	
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;

	ID3D11Buffer* matrixBuffer = nullptr;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	ID3D11SamplerState* samplerLinear = nullptr;

	VertexBuffers cubeBuffers;
	VertexBuffers monkeyBuffers;

	RenderData cube;
	RenderData monkey1;
	RenderData monkey2;

	bool createMatrixBuffer();
	bool createVertexBuffers(VertexBuffers& vertexBuffers, const std::string& modelFile);
	void renderObject(const RenderData& renderData);
};