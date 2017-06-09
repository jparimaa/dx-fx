#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Transformation.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/AssetManager.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <memory>

class ExampleApp : public fw::Application
{
public:
	ExampleApp();
	virtual ~ExampleApp();

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
	fw::Transformation trans;
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;
	fw::AssetManager assetManager;
	fw::AssetManager::VertexBuffer* vertexBuffer = nullptr;

	ID3D11Buffer* matrixBuffer = nullptr;	
	ID3D11ShaderResourceView* textureView = nullptr;
	ID3D11SamplerState* samplerLinear = nullptr;

	bool createMatrixBuffer();
};