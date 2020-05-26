#include "Framework.h"
#include "DX.h"
#include "Common.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <iostream>

namespace fw
{
Framework::Framework(LONG windowWidth, LONG windowHeight, Config cfg) :
    window(windowWidth, windowHeight),
    config(cfg)
{
}

Framework::~Framework()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool Framework::initialize(HINSTANCE hInstance, int nCmdShow)
{
    AllocConsole();
    freopen_s(&consoleStream, "CONOUT$", "w", stdout);
    freopen_s(&consoleStream, "CONOUT$", "w", stderr);

    if (!window.initialize(hInstance, nCmdShow))
    {
        return false;
    }
    if (!device.initialize(window.getHandle(), config))
    {
        return false;
    }

    input.initialize(window.getHandle());
    API::initialize(this);
    ImGuiContext* imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplWin32_Init(window.getHandle());
    ImGui_ImplDX11_Init(DX::device, DX::context);
    return true;
}

bool Framework::setApplication(Application* application)
{
    if (!application)
    {
        printError("Invalid application pointer");
        return false;
    }

    app = application;
    return app->initialize();
}

int Framework::execute()
{
    timer.start();
    MSG msg = {0};
    while (WM_QUIT != msg.message && running)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            input.update();
            app->update();
            timer.tick();
            input.lateUpdate();
            app->render();
            app->gui();
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            DX::swapChain->Present(1, 0);
        }
    }
    return static_cast<int>(msg.wParam);
}

void Framework::quit()
{
    running = false;
}

const Window* Framework::getWindow() const
{
    return &window;
}

const Timer* Framework::getTimer() const
{
    return &timer;
}

const Input* Framework::getInput() const
{
    return &input;
}

Device* Framework::getDevice()
{
    return &device;
}

} // namespace fw