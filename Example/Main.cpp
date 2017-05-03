#include "ExampleApp.h"
#include <Framework.h>
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int nCmdShow)
{
	fw::Framework framework;
	if (!framework.initialize(hInstance, nCmdShow)) {
		return 0;
	}
	ExampleApp app;
	framework.setApplication(&app);	
	int ret = framework.execute();
	return ret;
}
