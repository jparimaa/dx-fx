#pragma once

#include "Window.h"
#include "Device.h"
#include "Application.h"
#include "Timer.h"
#include <windows.h>
#include <cstdio>

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
	void quit();

	const Window& getWindow() const;
	const Timer& getTimer() const;
	
private:
	FILE* consoleStream;
	Window window;
	Device device;	
	Application* app = nullptr;
	Timer timer;
	bool running = true;
};

} // fw
