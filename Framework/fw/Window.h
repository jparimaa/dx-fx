#pragma once

#include <windows.h>

namespace fw
{

class Window
{
public:
	Window(LONG width, LONG height);
	~Window();
	bool initialize(HINSTANCE hInstance, int nCmdShow);

	HWND getHandle() const;
	LONG getWidth() const;
	LONG getHeight() const;

private:
	HINSTANCE instanceHandle = nullptr;
	HWND windowHandle = nullptr;
	LONG width = 800;
	LONG height = 600;
};

} // fw
