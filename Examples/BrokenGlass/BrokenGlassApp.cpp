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
float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
UINT sampleMask = 0xffffffff;

bool createShader(const std::string& filename, BrokenGlassApp::ShaderProgram& shaderProgram)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string shaderFile = ROOT_PATH + filename;
    fw::ToWchar wcharHelper(shaderFile);
    if (!shaderProgram.vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0", layout))
    {
        return false;
    }

    if (!shaderProgram.pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }
    return true;
}
} // namespace

BrokenGlassApp::BrokenGlassApp()
{
}

BrokenGlassApp::~BrokenGlassApp()
{
    fw::release(depthmapTexture);
    fw::release(depthmapDSV);
    fw::release(depthmapSRV);
    fw::release(sceneInputTexture);
    fw::release(sceneInputRTV);
    fw::release(sceneInputSRV);
    fw::release(outputTexture);
    fw::release(outputRTV);
    fw::release(outputSRV);
    fw::release(blendDisabledState);
    fw::release(cameraPositionBuffer);
}

bool BrokenGlassApp::initialize()
{
    bool status = true;
    status = createConstantBuffer(monkeyData.matrixBuffer, sizeof(MatrixData));
    assert(status);

    status = createConstantBuffer(cubeData.matrixBuffer, sizeof(MatrixData));
    assert(status);

    status = createConstantBuffer(cameraPositionBuffer, sizeof(CameraData));
    assert(status);

    status = assetManager.getLinearSampler(&samplerLinear);
    assert(status);

    status = createShader("Examples/BrokenGlass/simple.hlsl", diffuseShader);
    assert(status);

    status = createShader("Examples/BrokenGlass/warped.hlsl", warpedShader);
    assert(status);

    camera.getTransformation().position = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
    camera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
    cameraController.setCameraTransformation(&camera.getTransformation());

    monkeyData.transform.position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    monkeyData.transform.updateWorldMatrix();

    cubeData.transform.position = DirectX::XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
    cubeData.transform.scale = DirectX::XMVectorSet(1.0f, 1.0f, 0.05f, 1.0f);
    cubeData.transform.updateWorldMatrix();

    diffuseTextureView = assetManager.getTextureView(ROOT_PATH + std::string("/Assets/green_square.png"));
    assert(diffuseTextureView != nullptr);

    glassNormalTextureView = assetManager.getTextureView(ROOT_PATH + std::string("/Assets/broken_glass_normal.jpg"));
    assert(glassNormalTextureView != nullptr);

    glassAdditiveTextureView = assetManager.getTextureView(ROOT_PATH + std::string("/Assets/broken_glass_add.jpg"));
    assert(glassAdditiveTextureView != nullptr);

    monkeyVertexBuffer = assetManager.getVertexBuffer(ROOT_PATH + std::string("/Assets/monkey.3ds"));
    assert(monkeyVertexBuffer != nullptr);

    cubeVertexBuffer = assetManager.getVertexBuffer(ROOT_PATH + std::string("/Assets/cube.obj"));
    assert(cubeVertexBuffer != nullptr);

    status = createDepthMap();
    assert(status);

    status = createRenderTarget(sceneInputTexture, sceneInputRTV, sceneInputSRV);
    assert(status);

    status = createRenderTarget(outputTexture, outputRTV, outputSRV);
    assert(status);

    std::cout << "BrokenGlassApp initialization completed\n";

    viewport.Height = static_cast<float>(fw::API::getWindowHeight());
    viewport.Width = static_cast<float>(fw::API::getWindowWidth());
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    D3D11_BLEND_DESC blendStateDesc{};
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
    fw::DX::device->CreateBlendState(&blendStateDesc, &blendDisabledState);
    fw::DX::context->OMSetBlendState(blendDisabledState, blendFactor, sampleMask);

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
    updateMatrixBuffer(cubeData);

    DirectX::XMFLOAT4 camPos;
    DirectX::XMStoreFloat4(&camPos, camera.getTransformation().position);

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    fw::DX::context->Map(cameraPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    CameraData* cameraData = (CameraData*)MappedResource.pData;
    cameraData->cameraPos[0] = camPos.x;
    cameraData->cameraPos[1] = camPos.y;
    cameraData->cameraPos[2] = camPos.z;
    cameraData->viewProjection = DirectX::XMMatrixMultiply(camera.getViewMatrix(), camera.getProjectionMatrix());
    fw::DX::context->Unmap(cameraPositionBuffer, 0);
}

void BrokenGlassApp::render()
{
    fw::DX::context->ClearRenderTargetView(sceneInputRTV, clearColor);
    fw::DX::context->ClearRenderTargetView(outputRTV, clearColor);
    fw::DX::context->ClearDepthStencilView(depthmapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::VertexBuffer* vb = monkeyVertexBuffer;
    fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    fw::DX::context->IASetInputLayout(diffuseShader.vertexShader.getVertexLayout());
    fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    fw::DX::context->VSSetConstantBuffers(0, 1, &monkeyData.matrixBuffer);
    fw::DX::context->VSSetShader(diffuseShader.vertexShader.get(), nullptr, 0);
    D3D11_VIEWPORT viewports[2] = {viewport, viewport};
    fw::DX::context->RSSetViewports(2, viewports);
    fw::DX::context->PSSetShader(diffuseShader.pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);
    fw::DX::context->PSSetShaderResources(0, 1, &diffuseTextureView);
    ID3D11RenderTargetView* renderTargets[2] = {sceneInputRTV, outputRTV};
    fw::DX::context->OMSetRenderTargets(2, renderTargets, depthmapDSV);

    fw::DX::context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);

    vb = cubeVertexBuffer;
    fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    fw::DX::context->IASetInputLayout(warpedShader.vertexShader.getVertexLayout());
    fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    fw::DX::context->VSSetConstantBuffers(0, 1, &cubeData.matrixBuffer);
    fw::DX::context->VSSetShader(warpedShader.vertexShader.get(), nullptr, 0);
    fw::DX::context->RSSetViewports(1, &viewport);
    fw::DX::context->PSSetShader(warpedShader.pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetConstantBuffers(1, 1, &cameraPositionBuffer);
    fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);
    ID3D11ShaderResourceView* textures[3] = {sceneInputSRV, glassNormalTextureView, glassAdditiveTextureView};
    fw::DX::context->OMSetRenderTargets(1, &outputRTV, depthmapDSV);
    fw::DX::context->PSSetShaderResources(0, 3, textures);

    fw::DX::context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);

    ID3D11RenderTargetView* backBuffer = fw::API::getRenderTargetView();
    blitter.blit(outputSRV, backBuffer);

    // For GUI
    fw::DX::context->OMSetRenderTargets(1, &backBuffer, fw::API::getDepthStencilView());
}

void BrokenGlassApp::gui()
{
    ImGui::Text("Hello, world!");
}

bool BrokenGlassApp::createConstantBuffer(ID3D11Buffer*& matrixBuffer, UINT size)
{
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = size;
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

bool BrokenGlassApp::createDepthMap()
{
    D3D11_TEXTURE2D_DESC shadowmapDesc{};
    shadowmapDesc.Width = static_cast<UINT>(fw::API::getWindowWidth());
    shadowmapDesc.Height = static_cast<UINT>(fw::API::getWindowHeight());
    shadowmapDesc.MipLevels = 1;
    shadowmapDesc.ArraySize = 1;
    shadowmapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowmapDesc.SampleDesc = DXGI_SAMPLE_DESC{1, 0};
    shadowmapDesc.Usage = D3D11_USAGE_DEFAULT;
    shadowmapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadowmapDesc.CPUAccessFlags = 0;
    shadowmapDesc.MiscFlags = 0;
    fw::DX::device->CreateTexture2D(&shadowmapDesc, nullptr, &depthmapTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    fw::DX::device->CreateDepthStencilView(depthmapTexture, &depthStencilViewDesc, &depthmapDSV);

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    fw::DX::device->CreateShaderResourceView(depthmapTexture, &shaderResourceViewDesc, &depthmapSRV);

    return true;
}

bool BrokenGlassApp::createRenderTarget(ID3D11Texture2D*& texture, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv)
{
    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = fw::API::getWindowWidth();
    textureDesc.Height = fw::API::getWindowHeight();
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    fw::DX::device->CreateTexture2D(&textureDesc, NULL, &texture);

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    fw::DX::device->CreateRenderTargetView(texture, &renderTargetViewDesc, &rtv);

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = textureDesc.Format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = 1;
    fw::DX::device->CreateShaderResourceView(texture, &SRVDesc, &srv);

    return true;
}
