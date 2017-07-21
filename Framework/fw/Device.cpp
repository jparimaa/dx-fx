#include "Device.h"
#include "Common.h"
#include "DX.h"
#include <iostream>

namespace fw
{

Device::Device()
{
}

Device::~Device()
{
	if (immediateContext) {
		immediateContext->ClearState();
	}
	release(renderTargetView);
	release(swapChain);
	release(immediateContext);
	release(d3dDevice);
	release(depthStencil);
	release(depthStencilView);
}

bool Device::initialize(HWND windowHandle)
{
	RECT rc;
	GetClientRect(windowHandle, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
		
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = windowHandle;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = (UINT)D3D11_STANDARD_MULTISAMPLE_PATTERN;
	sd.Windowed = TRUE;

	HRESULT hr = S_OK;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
										   D3D11_SDK_VERSION, &sd, &swapChain, &d3dDevice, &featureLevel, &immediateContext);
		if (SUCCEEDED(hr)) {
			break;
		}
	}

	if (FAILED(hr)) {
		printError("Failed to create device and swap chain", &hr);
		return false;
	}

	ID3D11Texture2D* backBuffer = nullptr;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr)) {
		printError("Failed to get swap chain buffer", &hr);
		return false;
	}

	hr = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release();
	if (FAILED(hr)) {
		printError("Failed to create render target view", &hr);
		return false;
	}
	
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Width = width;
	depthDesc.Height = height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 4;
	depthDesc.SampleDesc.Quality = (UINT)D3D11_STANDARD_MULTISAMPLE_PATTERN;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	hr = d3dDevice->CreateTexture2D(&depthDesc, nullptr, &depthStencil);
	if (FAILED(hr)) {
		printError("Failed to create depth stencil texture", &hr);
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(DSVDesc));
	DSVDesc.Format = depthDesc.Format;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	DSVDesc.Texture2D.MipSlice = 0;
	hr = d3dDevice->CreateDepthStencilView(depthStencil, &DSVDesc, &depthStencilView);
	if (FAILED(hr)) {
		printError("Failed to create depth stencil view", &hr);
		return false;
	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports(1, &vp);

	DX::device = d3dDevice;
	DX::context = immediateContext;
	DX::renderTargetView = renderTargetView;
	DX::swapChain = swapChain;

	return true;
}

ID3D11DepthStencilView* Device::getDepthStencilView()
{
	return depthStencilView;
}

ID3D11RenderTargetView* Device::getRenderTargetView()
{
	return renderTargetView;
}

} // fw