#pragma once

#include "Window.h"
#include "Device.h"
#include "Application.h"
#include <windows.h>

namespace fw
{

class Framework
{
public:
	Framework();
	~Framework();

	bool initialize(HINSTANCE hInstance, int nCmdShow);
	bool setApplication(Application* application);
	int execute();

private:
	Window window;
	Device device;
	Application* app = nullptr;
};

} // fx