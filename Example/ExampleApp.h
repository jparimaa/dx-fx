#pragma once

#include <Application.h>
#include <VertexShader.h>
#include <PixelShader.h>
#include <d3d11.h>

class ExampleApp : public fw::Application
{
public:
	ExampleApp();
	virtual ~ExampleApp();
	
	virtual bool initialize();
	virtual void update();
	virtual void render();
	virtual void gui();

private:
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;
	ID3D11Buffer* vertexBuffer = nullptr;

	bool createBuffer();
};