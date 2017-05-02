#include "Framework.h"
#include "DX.h"
#include "Common.h"
#include <xnamath.h>
#include <vector>

namespace
{

float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

} // anonymous

namespace dfx
{

Framework::Framework()
{
}

Framework::~Framework()
{
	release(vertexBuffer);
}

bool Framework::initialize(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(window.initialize(hInstance, nCmdShow))) {
		return false;
	}

	if (FAILED(device.initialize(window.getWindowHandle()))) {
		return false;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(vertexShader.create(L"example.fx", "VS", "vs_4_0", layout))) {
		return false;
	}

	if (FAILED(pixelShader.create(L"example.fx", "PS", "ps_4_0"))) {
		return false;
	}

	if (!createBuffer()) {
		return false;
	}

	return true;
}

int Framework::execute()
{
	MSG msg = {0};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			render();
		}
	}
	return (int)msg.wParam;
}

bool Framework::createBuffer()
{
	XMFLOAT3 vertices[] = {
		XMFLOAT3(0.0f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f),
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT3) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertices;

	HRESULT hr = DX::device->CreateBuffer(&bd, &data, &vertexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	DX::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return true;
}

void Framework::render()
{
	DX::context->ClearRenderTargetView(DX::renderTargetView, clearColor);

	DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
	DX::context->PSSetShader(pixelShader.get(), nullptr, 0);
	DX::context->Draw(3, 0);

	DX::swapChain->Present(0, 0);
}

} // fx