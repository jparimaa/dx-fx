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
    std::string shaderFile = ROOT_PATH + std::string("Examples/SDF/main.hlsl");
    fw::ToWchar wcharHelper(shaderFile);
    if (!m_vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0"))
    {
        return false;
    }

    m_shaderReloader.init(fw::DX::device, shaderFile, "PS", "ps_4_0");

    bool ok = createConstantBuffer();
    assert(ok);

    m_cameraController.setCameraTransformation(&m_camera.getTransformation());

    m_sphere1Transform.move(DirectX::XMFLOAT3(0.0f, 0.0f, 15.0f));
    m_sphere2Transform.move(DirectX::XMFLOAT3(0.0f, 0.0f, 15.0f));
    m_sphere3Transform.move(DirectX::XMFLOAT3(0.0f, 0.0f, 15.0f));
    m_sphereBoxTransform.move(DirectX::XMFLOAT3(0.0f, -5.0f, 15.0f));

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

    const float time = fw::API::getTimeSinceStart();
    const float delta = fw::API::getTimeDelta();
    const float movement = std::sin(time) * delta * 4.0f;

    m_cameraController.update();

    m_sphere1Transform.move(DirectX::XMFLOAT3(movement, 0.0f, 0.0f));
    m_sphere2Transform.move(DirectX::XMFLOAT3(-movement, 0.0f, 0.0f));
    m_sphere3Transform.move(DirectX::XMFLOAT3(0.0f, movement, 0.0f));
    m_sphereBoxTransform.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * delta * 0.1f);

    m_constants.time = time;
    m_constants.cameraTransform = m_camera.getTransformation().updateWorldMatrix();
    m_constants.sphere1Transform = DirectX::XMMatrixInverse(nullptr, m_sphere1Transform.updateWorldMatrix());
    m_constants.sphere2Transform = DirectX::XMMatrixInverse(nullptr, m_sphere2Transform.updateWorldMatrix());
    m_constants.sphere3Transform = DirectX::XMMatrixInverse(nullptr, m_sphere3Transform.updateWorldMatrix());
    m_constants.sphereBoxTransform = DirectX::XMMatrixInverse(nullptr, m_sphereBoxTransform.updateWorldMatrix());

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    Constants* constantsData = (Constants*)MappedResource.pData;
    *constantsData = m_constants;
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
    DirectX::XMFLOAT3 p;
    DirectX::XMStoreFloat3(&p, m_camera.getTransformation().position);
    ImGui::Text("%f, %f, %f", p.x, p.y, p.z);
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
