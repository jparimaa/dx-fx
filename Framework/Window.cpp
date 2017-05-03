#include "Window.h"

namespace
{

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

} //anonymous

namespace fw
{

Window::Window()
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
		return E_FAIL;
	}

	instanceHandle = hInstance;
	RECT rc = {0, 0, 800, 600};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	windowHandle = CreateWindow(L"dxfx", L"Direct3D 11", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
								rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

	if (!windowHandle) {
		return E_FAIL;
	}

	ShowWindow(windowHandle, nCmdShow);

	return S_OK;
}

HWND Window::getWindowHandle()
{
	return windowHandle;
}

} // fx