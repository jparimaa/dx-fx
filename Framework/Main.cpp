#include "Framework.h"
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int nCmdShow)
{
	Framework framework;
	if (!framework.initialize(hInstance, nCmdShow)) {
		return 0;
	}

	int ret = framework.execute();
	return ret;
}
