#include "MultithreadApp.h"
#include <fw/Common.h>
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int nCmdShow)
{
    return fw::executeGenericMain<MultithreadApp>(hInstance, nCmdShow, 800, 600);
}
