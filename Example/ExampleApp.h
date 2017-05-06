#pragma once

#include <Application.h>
#include <VertexShader.h>
#include <PixelShader.h>
#include <Transformation.h>
#include <Camera.h>
#include <windows.h>
#include <xnamath.h>
#include <d3d11.h>

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
	fw::Camera camera;
	fw::Transformation trans;
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* matrixBuffer = nullptr;

	bool createBuffer();
};