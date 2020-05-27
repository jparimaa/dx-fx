#include "TAAApp.h"
#include <fw/Common.h>
#include <fw/Config.h>
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int nCmdShow)
{
    fw::Config config{};
    config.sampleCount = 1;
    return fw::executeGenericMain<TAAApp>(hInstance, nCmdShow, 800, 600, config);
}
