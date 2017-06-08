#include "Framework.h"
#include "DX.h"
#include "Common.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include <iostream>

namespace fw
{

Framework::Framework(LONG windowWidth, LONG windowHeight) :
	window(windowWidth, windowHeight)
{
}

Framework::~Framework()
{
	ImGui_ImplDX11_Shutdown();
}

bool Framework::initialize(HINSTANCE hInstance, int nCmdShow)
{
	AllocConsole();
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);

	if (!window.initialize(hInstance, nCmdShow)) {
		return false;
	}
	if (!device.initialize(window.getHandle())) {
		return false;
	}

	input.initialize(window.getHandle());
	API::initialize(this);
	ImGui_ImplDX11_Init(window.getHandle(), DX::device, DX::context);
	return true;
}

bool Framework::setApplication(Application* application)
{
	if (!application) {
		printError("Invalid application pointer");
		return false;
	}

	app = application;	
	return app->initialize();
}

int Framework::execute()
{
	timer.start();
	MSG msg = {0};
	while (WM_QUIT != msg.message && running) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			ImGui_ImplDX11_NewFrame();
			input.update();
			app->update();
			timer.tick();
			input.lateUpdate();
			app->render();
			app->gui();
			ImGui::Render();
			DX::swapChain->Present(0, 0);
		}
	}	
	return static_cast<int>(msg.wParam);
}

void Framework::quit()
{
	running = false;
}

const Window* Framework::getWindow() const
{
	return &window;
}

const Timer* Framework::getTimer() const
{
	return &timer;
}

const Input* Framework::getInput() const
{
	return &input;
}

Device* Framework::getDevice()
{
	return &device;
}

} // fw