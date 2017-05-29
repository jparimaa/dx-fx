#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
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
	fw::Camera camera;
	fw::CameraController cameraController;
	fw::Transformation trans;
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* matrixBuffer = nullptr;
	unsigned int numIndices = 0;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	ID3D11SamplerState* samplerLinear = nullptr;

	bool createBuffer();
};