#pragma once

#include "Window.h"
#include "Device.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <d3d11.h>
#include <windows.h>

namespace dfx
{

class Framework
{
public:
	Framework();
	~Framework();

	bool initialize(HINSTANCE hInstance, int nCmdShow);
	int execute();

private:
	Window window;
	Device device;
	VertexShader vertexShader;
	PixelShader pixelShader;
	ID3D11Buffer* vertexBuffer = nullptr;
	
	bool createBuffer();
	void render();
};

} // fx