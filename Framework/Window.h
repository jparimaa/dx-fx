#pragma once

#include <windows.h>

namespace dfx
{

class Window
{
public:
	Window();
	~Window();
	HRESULT initialize(HINSTANCE hInstance, int nCmdShow);

	HWND getWindowHandle();

private:
	HINSTANCE instanceHandle = nullptr;
	HWND windowHandle = nullptr;
};

} // fx