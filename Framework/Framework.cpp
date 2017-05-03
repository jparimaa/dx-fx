#include "Framework.h"

namespace fw
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

	return true;
}

void Framework::setApplication(Application* application)
{
	app = application;
}

int Framework::execute()
{
	app->initialize();
	MSG msg = {0};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			app->update();
			app->render();
			app->gui();
		}
	}
	return (int)msg.wParam;
}

} // fx