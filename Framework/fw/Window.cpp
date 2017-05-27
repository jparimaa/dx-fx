#include "Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <iostream>

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam);

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;	
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

} //anonymous

namespace fw
{

Window::Window(LONG width, LONG height) :
	width(width),
	height(height)
{
}

Window::~Window()
{
}

HRESULT Window::initialize(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"dxfx";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		std::cerr << "ERROR: Could not register window class\n";
		return E_FAIL;
	}

	instanceHandle = hInstance;
	RECT rc = {0, 0, width, height};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	windowHandle = CreateWindow(L"dxfx", L"Direct3D 11", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
								rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

	if (!windowHandle) {
		std::cerr << "ERROR: Could not create window\n";
		return E_FAIL;
	}

	ShowWindow(windowHandle, nCmdShow);

	return S_OK;
}

HWND Window::getHandle() const
{
	return windowHandle;
}

LONG Window::getWidth() const
{
	return width;
}

LONG Window::getHeight() const
{
	return height;
}

} // fw