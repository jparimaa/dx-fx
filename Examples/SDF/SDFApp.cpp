#include "SDFApp.h"

#include <fw/Common.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>

#include <iostream>

namespace
{
const float clearColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
} // namespace

bool SDFApp::initialize()
{
    std::string shaderFile = ROOT_PATH + std::string("Examples/SDF/sdf.hlsl");
    fw::ToWchar wcharHelper(shaderFile);
    if (!m_vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0"))
    {
        return false;
    }

    m_shaderReloader.init(fw::DX::device, shaderFile, "PS", "ps_4_0");

    std::cout << "SDFApp initialization completed\n";

    return true;
}

void SDFApp::update()
{
    m_shaderReloader.recompileIfChanged();
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }
}

void SDFApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    fw::DX::context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    fw::DX::context->PSSetShader(m_shaderReloader.getShader(), nullptr, 0);
    fw::DX::context->Draw(3, 0);
}

void SDFApp::gui()
{
    ImGui::Text("SDF");
}
