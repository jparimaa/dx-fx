#include "VolumetricExplosionApp.h"
#include <fw/Model.h>
#include <fw/Common.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <vector>
#include <iostream>

namespace
{
float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

} // namespace

VolumetricExplosionApp::VolumetricExplosionApp()
{
}

VolumetricExplosionApp::~VolumetricExplosionApp()
{
    fw::release(m_matrixBuffer);
}

bool VolumetricExplosionApp::initialize()
{
    //DirectX::CreateDDSTextureFromFile();
    const std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string shaderFile = ROOT_PATH + std::string("Examples/VolumetricExplosion/example.hlsl");
    fw::ToWchar wcharHelper(shaderFile);
    if (!m_vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0", layout))
    {
        return false;
    }

    if (!m_pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }

    if (!createMatrixBuffer())
    {
        return false;
    }

    m_camera.getTransformation().position = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
    m_camera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
    m_cameraController.setCameraTransformation(&m_camera.getTransformation());

    m_transformation.position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    m_transformation.updateWorldMatrix();

    std::cout << "VolumetricExplosionApp initialization completed\n";

    return true;
}

void VolumetricExplosionApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }

    m_cameraController.update();
    m_camera.updateViewMatrix();

    m_transformation.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.1f);
    m_transformation.updateWorldMatrix();
}

void VolumetricExplosionApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::DX::context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    fw::DX::context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &m_linearSampler);

    fw::DX::context->IASetInputLayout(m_vertexShader.getVertexLayout());
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    MatrixData* matrixData = (MatrixData*)MappedResource.pData;
    matrixData->world = m_transformation.getWorldMatrix();
    matrixData->view = m_camera.getViewMatrix();
    matrixData->projection = m_camera.getProjectionMatrix();
    fw::DX::context->Unmap(m_matrixBuffer, 0);

    fw::DX::context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
}

void VolumetricExplosionApp::gui()
{
    ImGui::Text("Hello, world!");
}

bool VolumetricExplosionApp::createMatrixBuffer()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(DirectX::XMMATRIX) * 3;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = fw::DX::device->CreateBuffer(&bd, nullptr, &m_matrixBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create matrix buffer", &hr);
        return false;
    }
    return true;
}
