#pragma once

#include <windows.h>
#include <d3d11.h>

namespace dfx
{

class Device
{
public:
	Device();
	~Device();

	HRESULT initialize(HWND windowHandle);

private:
	ID3D11DeviceContext* immediateContext = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* d3dDevice = nullptr;
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};

} // fx