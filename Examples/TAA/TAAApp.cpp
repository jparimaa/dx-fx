#include "TAAApp.h"
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
float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

float getHaltonSequenceValue(int index, int base)
{
    float result = 0.0f;
    float fraction = 1.0f / static_cast<float>(base);

    while (index > 0)
    {
        result += static_cast<float>((index % base) * fraction);

        index /= base;
        fraction /= static_cast<float>(base);
    }

    return result;
}
} // namespace

TAAApp::~TAAApp()
{
    fw::release(m_matrixBuffer);
}

bool TAAApp::initialize()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string shaderFile = ROOT_PATH + std::string("Examples/TAA/render.hlsl");
    fw::ToWchar wcharHelper(shaderFile);

    bool ok = true;
    ok = ok && m_vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0", layout);
    ok = ok && m_pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0");
    ok = ok && createMatrixBuffers();
    ok = ok && m_assetManager.getLinearSampler(&m_samplerLinear);
    ok = ok && createPrevFrameTexture();
    ok = ok && createMotionTexture();
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

    std::cout << "TAAApp initialization completed\n";

    return true;
}

void TAAApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }

    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        fw::DX::context->Map(m_prevMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
        MatrixData* matrixData = (MatrixData*)MappedResource.pData;
        matrixData->world = m_transformation.getWorldMatrix();
        matrixData->view = m_camera.getViewMatrix();
        matrixData->projection = m_camera.getProjectionMatrix();
        fw::DX::context->Unmap(m_prevMatrixBuffer, 0);
    }

    m_cameraController.update();
    m_camera.updateViewMatrix();

    //m_transformation.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.1f);
    m_transformation.updateWorldMatrix();

    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        fw::DX::context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
        MatrixData* matrixData = (MatrixData*)MappedResource.pData;
        matrixData->world = m_transformation.getWorldMatrix();
        matrixData->view = m_camera.getViewMatrix();
        matrixData->projection = m_camera.getProjectionMatrix();
        fw::DX::context->Unmap(m_matrixBuffer, 0);
    }
}

void TAAApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::VertexBuffer* vb = m_vertexBuffer;
    fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    fw::DX::context->IASetInputLayout(m_vertexShader.getVertexLayout());
    fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    fw::DX::context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    fw::DX::context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
    fw::DX::context->VSSetConstantBuffers(1, 1, &m_prevMatrixBuffer);

    fw::DX::context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &m_samplerLinear);
    fw::DX::context->PSSetShaderResources(0, 1, &m_textureView);

    fw::DX::context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);
}

void TAAApp::gui()
{
    ImGui::Text("Hello, world!");
}

bool TAAApp::createMatrixBuffers()
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

    hr = fw::DX::device->CreateBuffer(&bd, nullptr, &m_prevMatrixBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev matrix buffer", &hr);
        return false;
    }
    return true;
}

bool TAAApp::createPrevFrameTexture()
{
    ID3D11Device* device = fw::DX::device;

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = fw::API::getWindowWidth();
    textureDesc.Height = fw::API::getWindowWidth();
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &m_prevFrameTexture);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev frame texture", &hr);
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = textureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    hr = device->CreateRenderTargetView(m_prevFrameTexture, &rtvDesc, &m_prevFrameRtv);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev frame RTV", &hr);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(m_prevFrameTexture, &srvDesc, &m_prevFrameSrv);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev frame SRV", &hr);
        return false;
    }

    return true;
}

bool TAAApp::createMotionTexture()
{
    ID3D11Device* device = fw::DX::device;

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = fw::API::getWindowWidth();
    textureDesc.Height = fw::API::getWindowWidth();
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &m_motionTexture);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev frame texture", &hr);
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = textureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    hr = device->CreateRenderTargetView(m_motionTexture, &rtvDesc, &m_motionRtv);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev frame RTV", &hr);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(m_motionTexture, &srvDesc, &m_motionSrv);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev frame SRV", &hr);
        return false;
    }

    return true;
}
