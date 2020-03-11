#include "BrokenGlassApp.h"

#include <fw/Model.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>

#include <vector>
#include <iostream>

namespace
{
float clearColor[4] = {0.0f, 0.0f, 0.1f, 1.0f};
} // namespace

BrokenGlassApp::BrokenGlassApp()
{
}

BrokenGlassApp::~BrokenGlassApp()
{
}

bool BrokenGlassApp::initialize()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string shaderFile = ROOT_PATH + std::string("Examples/Multithread/simple.hlsl");
    fw::ToWchar wcharHelper(shaderFile);
    if (!vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0", layout))
    {
        return false;
    }

    if (!pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }

    if (!createMatrixBuffer(monkeyData.matrixBuffer))
    {
        return false;
    }

    if (!assetManager.getLinearSampler(&samplerLinear))
    {
        return false;
    }

    camera.getTransformation().position = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
    camera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
    cameraController.setCameraTransformation(&camera.getTransformation());

    monkeyData.transform.position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    monkeyData.transform.updateWorldMatrix();

    textureView = assetManager.getTextureView(ROOT_PATH + std::string("/Assets/green_square.png"));
    vertexBuffer = assetManager.getVertexBuffer(ROOT_PATH + std::string("/Assets/monkey.3ds"));
    assert(textureView != nullptr);
    assert(vertexBuffer != nullptr);

    std::cout << "BrokenGlassApp initialization completed\n";

    viewport.Height = static_cast<float>(fw::API::getWindowHeight());
    viewport.Width = static_cast<float>(fw::API::getWindowWidth());
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    return true;
}

void BrokenGlassApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }

    cameraController.update();
    camera.updateViewMatrix();

    monkeyData.transform.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.1f);
    monkeyData.transform.updateWorldMatrix();

    updateMatrixBuffer(monkeyData);
}

void BrokenGlassApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::VertexBuffer* vb = vertexBuffer;
    fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    fw::DX::context->IASetInputLayout(vertexShader.getVertexLayout());
    fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    fw::DX::context->VSSetConstantBuffers(0, 1, &monkeyData.matrixBuffer);
    fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
    fw::DX::context->RSSetViewports(1, &viewport);
    fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);
    fw::DX::context->PSSetShaderResources(0, 1, &textureView);
    ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
    fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());

    fw::DX::context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);

    /*
    ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
    fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());
    */
}

void BrokenGlassApp::gui()
{
    ImGui::Text("Hello, world!");
}

bool BrokenGlassApp::createMatrixBuffer(ID3D11Buffer*& matrixBuffer)
{
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(DirectX::XMMATRIX) * 3;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = fw::DX::device->CreateBuffer(&bd, nullptr, &matrixBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create matrix buffer", &hr);
        return false;
    }
    return true;
}

void BrokenGlassApp::updateMatrixBuffer(const TransformData& td)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(td.matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    MatrixData* matrixData = (MatrixData*)MappedResource.pData;
    matrixData->world = td.transform.getWorldMatrix();
    matrixData->view = camera.getViewMatrix();
    matrixData->projection = camera.getProjectionMatrix();
    fw::DX::context->Unmap(td.matrixBuffer, 0);
}
