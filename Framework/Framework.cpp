#include "Framework.h"
#include "DX.h"
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

void Framework::render()
{
	DX::context->ClearRenderTargetView(DX::renderTargetView, clearColor);
	DX::swapChain->Present(0, 0);
}

} // fx