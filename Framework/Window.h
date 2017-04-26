#pragma once
#include <windows.h>

class Window
{
public:
	Window();
	~Window();
	HRESULT initialize(HINSTANCE hInstance, int nCmdShow);

	HWND getWindowHandle();

private:
	HINSTANCE instanceHandle = NULL;
	HWND windowHandle = NULL;
};