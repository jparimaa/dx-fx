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
	Framework(LONG windowWidth, LONG windowHeight);
	~Framework();

	bool initialize(HINSTANCE hInstance, int nCmdShow);
	bool setApplication(Application* application);
	int execute();

	Window* getWindow();

private:
	Window window;
	Device device;
	Application* app = nullptr;
};

} // fw
