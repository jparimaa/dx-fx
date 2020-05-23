#pragma once

#include "Config.h"

#include <windows.h>
#include <d3d11.h>

namespace fw
{
class Device
{
public:
    Device();
    ~Device();

    bool initialize(HWND windowHandle, Config config);
    ID3D11DepthStencilView* getDepthStencilView();
    ID3D11RenderTargetView* getRenderTargetView();

private:
    ID3D11DeviceContext* immediateContext = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* d3dDevice = nullptr;
    ID3D11Texture2D* depthStencil = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};

} // namespace fw