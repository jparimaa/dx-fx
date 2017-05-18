#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Transformation.h>
#include <fw/Camera.h>
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
	fw::API* api = nullptr;
	fw::Camera camera;
	fw::Transformation trans;
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* matrixBuffer = nullptr;
	unsigned int numIndices = 0;

	bool createBuffer();
};