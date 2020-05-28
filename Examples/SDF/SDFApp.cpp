#include "SDFApp.h"
#include <fw/Model.h>
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
const float clearColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
} // namespace

SDFApp::~SDFApp()
{
    fw::release(m_matrixBuffer);
}

bool SDFApp::initialize()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> renderLayout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string renderShaderFile = ROOT_PATH + std::string("Examples/SDF/sdf.hlsl");
    fw::ToWchar renderShaderWchar(renderShaderFile);

    bool ok = true;
    ok = ok && m_renderVS.create(renderShaderWchar.getWchar(), "VS", "vs_4_0", renderLayout);
    ok = ok && m_renderPS.create(renderShaderWchar.getWchar(), "PS", "ps_4_0");
    ok = ok && createConstantBuffers();
    ok = ok && m_assetManager.getLinearSampler(&m_linearSampler);
    assert(ok);

    m_camera.getTransformation().position = DirectX::XMVectorSet(0.0f, 1.5f, -3.0f, 0.0f);
    m_camera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
    m_cameraController.setCameraTransformation(&m_camera.getTransformation());

    m_transformation.position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    m_transformation.updateWorldMatrix();

    m_textureView = m_assetManager.getTextureView(ROOT_PATH + std::string("/Assets/green_square.png"));
    m_vertexBuffer = m_assetManager.getVertexBuffer(ROOT_PATH + std::string("/Assets/monkey.3ds"));
    assert(m_textureView != nullptr);
    assert(m_vertexBuffer != nullptr);

    std::cout << "SDFApp initialization completed\n";

    return true;
}

void SDFApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }

    m_cameraController.update();
    m_camera.updateViewMatrix();

    m_transformation.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.3f);
    m_transformation.updateWorldMatrix();

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    MatrixData* matrixData = (MatrixData*)MappedResource.pData;
    matrixData->world = m_transformation.getWorldMatrix();
    matrixData->view = m_camera.getViewMatrix();
    matrixData->projection = m_camera.getProjectionMatrix();
    fw::DX::context->Unmap(m_matrixBuffer, 0);
}

void SDFApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::VertexBuffer* vb = m_vertexBuffer;
    fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    fw::DX::context->IASetInputLayout(m_renderVS.getVertexLayout());
    fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    fw::DX::context->VSSetShader(m_renderVS.get(), nullptr, 0);
    fw::DX::context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
    fw::DX::context->PSSetShader(m_renderPS.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &m_linearSampler);
    fw::DX::context->PSSetShaderResources(0, 1, &m_textureView);

    fw::DX::context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);
}

void SDFApp::gui()
{
    ImGui::Text("SDF");
}

bool SDFApp::createConstantBuffers()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(MatrixData);
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
