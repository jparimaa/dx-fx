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

    bool ok = createConstantBuffer();
    assert(ok);

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

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    Constants* constantsData = (Constants*)MappedResource.pData;
    constantsData->time = fw::API::getTimeSinceStart();
    fw::DX::context->Unmap(m_constantBuffer, 0);
}

void SDFApp::render()
{
    ID3D11DeviceContext* context = fw::DX::context;
    context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    context->PSSetShader(m_shaderReloader.getShader(), nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_constantBuffer);
    context->Draw(3, 0);
}

void SDFApp::gui()
{
    ImGui::Text("SDF");
}

bool SDFApp::createConstantBuffer()
{
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(Constants);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = fw::DX::device->CreateBuffer(&bd, nullptr, &m_constantBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create constant buffer", &hr);
        return false;
    }
    return true;
}
