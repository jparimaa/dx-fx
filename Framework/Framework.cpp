#include "Framework.h"

namespace fw
{

Framework::Framework(LONG windowWidth, LONG windowHeight) :
	window(windowWidth, windowHeight)
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

bool Framework::setApplication(Application* application)
{
	if (!application) {
		return false;
	}

	app = application;
	app->initializeAPI(this);
	return app->initialize();
}

int Framework::execute()
{
	timer.start();
	MSG msg = {0};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {			
			app->update();
			timer.tick();
			app->render();
			app->gui();			
		}
	}
	return (int)msg.wParam;
}

const Window& Framework::getWindow() const
{
	return window;
}

const Timer& Framework::getTimer() const
{
	return timer;
}

} // fw