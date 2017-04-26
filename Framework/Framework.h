#pragma once

#include "Window.h"
#include "Device.h"
#include <d3d11.h>
#include <windows.h>

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
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	void render();
};