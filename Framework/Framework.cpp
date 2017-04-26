#include "Framework.h"

namespace
{

float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

} // anonymous

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
	return true;
}

int Framework::execute()
{
	context = device.getContext();
	renderTargetView = device.getRenderTargetView();
	swapChain = device.getSwapChain();

	MSG msg = {0};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
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
	context->ClearRenderTargetView(renderTargetView, clearColor);
	swapChain->Present(0, 0);
}
