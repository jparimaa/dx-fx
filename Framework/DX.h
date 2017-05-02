#pragma once

#include <d3d11.h>

namespace dfx
{

class DX
{
public:
	DX() = delete;
	static ID3D11DeviceContext* context;
	static ID3D11RenderTargetView* renderTargetView;
	static IDXGISwapChain* swapChain;
	static ID3D11Device* device;
};

} // fx