#pragma once

#include <windows.h>
#include <d3d11.h>

class Device
{
public:
	Device();
	~Device();

	HRESULT initialize(HWND windowHandle);

private:
	ID3D11DeviceContext* immediateContext = NULL;
	ID3D11RenderTargetView* renderTargetView = NULL;
	IDXGISwapChain* swapChain = NULL;
	ID3D11Device* d3dDevice = NULL;
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};