#include "FullscreenApp.h"
#include <fw/Common.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>
#include <WICTextureLoader.h>
#include <vector>
#include <iostream>

namespace
{
float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};
} // namespace

FullscreenApp::FullscreenApp()
{
}

FullscreenApp::~FullscreenApp()
{
}

bool FullscreenApp::initialize()
{
    std::string shaderFile = ROOT_PATH + std::string("Examples/Fullscreen/example.hlsl");
    fw::ToWchar wcharHelper(shaderFile);
    if (!vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0"))
    {
        return false;
    }

    if (!pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }

    if (!assetManager.getLinearSampler(&samplerLinear))
    {
        return false;
    }

    textureView = assetManager.getTextureView(ROOT_PATH + std::string("/Assets/broken_glass_add.jpg"));
    assert(textureView != nullptr);

    std::cout << "FullscreenApp initialization completed\n";

    return true;
}

void FullscreenApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }
}

void FullscreenApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
    fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);

    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    fw::DX::context->PSSetShaderResources(0, 1, &textureView);
    fw::DX::context->Draw(3, 0);
}

void FullscreenApp::gui()
{
}
