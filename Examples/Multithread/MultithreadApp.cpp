#include "MultithreadApp.h"

#include <fw/Model.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>

#include <vector>
#include <iostream>
#include <thread>

namespace
{
float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};
} // namespace

MultithreadApp::MultithreadApp()
{
}

MultithreadApp::~MultithreadApp()
{
    fw::release(deferredContext1);
    fw::release(deferredContext2);
}

bool MultithreadApp::initialize()
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

    if (!createMatrixBuffer(monkeyData1.matrixBuffer) || !createMatrixBuffer(monkeyData2.matrixBuffer))
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

    monkeyData1.transform.position = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
    monkeyData1.transform.updateWorldMatrix();
    monkeyData2.transform.position = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    monkeyData2.transform.updateWorldMatrix();

    textureView = assetManager.getTextureView(ROOT_PATH + std::string("/Assets/green_square.png"));
    vertexBuffer = assetManager.getVertexBuffer(ROOT_PATH + std::string("/Assets/monkey.3ds"));
    assert(textureView != nullptr);
    assert(vertexBuffer != nullptr);

    std::cout << "MultithreadApp initialization completed\n";

    D3D11_FEATURE_DATA_THREADING featureData{};
    HRESULT hr = fw::DX::device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &featureData, sizeof(featureData));
    assert(SUCCEEDED(hr));
    std::cout << "Concurrent resource creation supported: " << featureData.DriverConcurrentCreates << "\n"
              << "Command lists supported: " << featureData.DriverCommandLists << "\n";

    hr = fw::DX::device->CreateDeferredContext(0, &deferredContext1);
    assert(SUCCEEDED(hr));
    hr = fw::DX::device->CreateDeferredContext(0, &deferredContext2);
    assert(SUCCEEDED(hr));

    viewport.Height = static_cast<float>(fw::API::getWindowHeight());
    viewport.Width = static_cast<float>(fw::API::getWindowWidth());
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    return true;
}

void MultithreadApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }

    cameraController.update();
    camera.updateViewMatrix();

    monkeyData1.transform.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.1f);
    monkeyData1.transform.updateWorldMatrix();

    monkeyData2.transform.rotate(DirectX::XMFLOAT3(0.5f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * -0.05f);
    monkeyData2.transform.updateWorldMatrix();

    updateMatrixBuffer(monkeyData1);
    updateMatrixBuffer(monkeyData2);
}

void MultithreadApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    ID3D11CommandList* commandList1 = nullptr;
    ID3D11CommandList* commandList2 = nullptr;

    // It doesn't make much sense to launch threads on every render call, thread pool would be better
    // Also creating a deferred context for a single draw context is inefficient
    // Or using multithreaded command list generation for two draw calls at all
    // Or recreating the same command list every time
    // But for the sake of simplicity & example, let's go with this
    std::thread t1(&MultithreadApp::renderMonkey, this, deferredContext1, std::ref(commandList1), std::ref(monkeyData1));
    std::thread t2(&MultithreadApp::renderMonkey, this, deferredContext2, std::ref(commandList2), std::ref(monkeyData2));

    t1.join();
    fw::DX::context->ExecuteCommandList(commandList1, false);
    t2.join();
    fw::DX::context->ExecuteCommandList(commandList2, false);

    fw::release(commandList1);
    fw::release(commandList2);

    // For GUI
    ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
    fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());
}

void MultithreadApp::gui()
{
    ImGui::Text("Hello, world!");
}

bool MultithreadApp::createMatrixBuffer(ID3D11Buffer*& matrixBuffer)
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

void MultithreadApp::updateMatrixBuffer(const TransformData& td)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(td.matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    MatrixData* matrixData = (MatrixData*)MappedResource.pData;
    matrixData->world = td.transform.getWorldMatrix();
    matrixData->view = camera.getViewMatrix();
    matrixData->projection = camera.getProjectionMatrix();
    fw::DX::context->Unmap(td.matrixBuffer, 0);
}

void MultithreadApp::renderMonkey(ID3D11DeviceContext* context, ID3D11CommandList*& commandList, const TransformData& t)
{
    fw::VertexBuffer* vb = vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    context->IASetInputLayout(vertexShader.getVertexLayout());
    context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetConstantBuffers(0, 1, &t.matrixBuffer);
    context->VSSetShader(vertexShader.get(), nullptr, 0);
    context->RSSetViewports(1, &viewport);
    context->PSSetShader(pixelShader.get(), nullptr, 0);
    context->PSSetSamplers(0, 1, &samplerLinear);
    context->PSSetShaderResources(0, 1, &textureView);
    ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
    context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());

    context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);

    HRESULT hr = context->FinishCommandList(false, &commandList);
    assert(SUCCEEDED(hr));
}
