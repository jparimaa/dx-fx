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
const float clearColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const float clearMotion[2] = {0.0f, 0.0f};

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

float getWidthOffset(int index)
{
    const float halton = getHaltonSequenceValue(index, 2);
    const float offset = (halton - 0.5f) / static_cast<float>(fw::API::getWindowWidth()) * 2.0f;
    return offset;
}

float getHeightOffset(int index)
{
    const float halton = getHaltonSequenceValue(index, 3);
    const float offset = (halton - 0.5f) / static_cast<float>(fw::API::getWindowHeight()) * 2.0f;
    return offset;
}

DirectX::XMFLOAT4X4 identity4x4()
{
    DirectX::XMMATRIX mat = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT4X4 float4x4;
    DirectX::XMStoreFloat4x4(&float4x4, mat);
    return float4x4;
}
} // namespace

TAAApp::~TAAApp()
{
    fw::release(m_matrixBuffer);
}

bool TAAApp::initialize()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> renderLayout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string renderShaderFile = ROOT_PATH + std::string("Examples/TAA/render.hlsl");
    fw::ToWchar renderShaderWchar(renderShaderFile);

    std::string taaShaderFile = ROOT_PATH + std::string("Examples/TAA/TAA.hlsl");
    fw::ToWchar taaShaderWchar(taaShaderFile);

    bool ok = true;
    ok = ok && m_renderVS.create(renderShaderWchar.getWchar(), "VS", "vs_4_0", renderLayout);
    ok = ok && m_renderPS.create(renderShaderWchar.getWchar(), "PS", "ps_4_0");
    ok = ok && m_taaVS.create(taaShaderWchar.getWchar(), "VS", "vs_4_0");
    ok = ok && m_taaPS.create(taaShaderWchar.getWchar(), "PS", "ps_4_0");
    ok = ok && createConstantBuffers();
    ok = ok && m_assetManager.getLinearSampler(&m_linearSampler);
    ok = ok && createFrameTextures();
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

    // Prev frame
    DirectX::XMFLOAT4X4 jitter = identity4x4();
    jitter(2, 0) = m_disableJitter ? 0.0f : m_jitterX;
    jitter(2, 1) = m_disableJitter ? 0.0f : m_jitterY;
    DirectX::XMMATRIX jitterMatrix = DirectX::XMLoadFloat4x4(&jitter);

    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        fw::DX::context->Map(m_prevMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
        MatrixData* matrixData = (MatrixData*)MappedResource.pData;
        matrixData->world = m_transformation.getWorldMatrix();
        matrixData->view = m_camera.getViewMatrix();
        matrixData->projection = m_camera.getProjectionMatrix();
        matrixData->jitter = jitterMatrix;
        fw::DX::context->Unmap(m_prevMatrixBuffer, 0);
    }

    // Update
    m_cameraController.update();
    m_camera.updateViewMatrix();

    m_transformation.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.3f);
    m_transformation.updateWorldMatrix();

    ++m_haltonIndex;
    m_haltonIndex = m_haltonIndex % 8;

    // Current frame
    m_jitterX = getWidthOffset(m_haltonIndex);
    m_jitterY = getHeightOffset(m_haltonIndex);
    jitter(2, 0) = m_disableJitter ? 0.0f : m_jitterX;
    jitter(2, 1) = m_disableJitter ? 0.0f : m_jitterY;
    jitterMatrix = DirectX::XMLoadFloat4x4(&jitter);

    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        fw::DX::context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
        MatrixData* matrixData = (MatrixData*)MappedResource.pData;
        matrixData->world = m_transformation.getWorldMatrix();
        matrixData->view = m_camera.getViewMatrix();
        matrixData->projection = m_camera.getProjectionMatrix();
        matrixData->jitter = jitterMatrix;
        fw::DX::context->Unmap(m_matrixBuffer, 0);
    }

    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        fw::DX::context->Map(m_taaParametersBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
        TAAParameters* taaParameters = (TAAParameters*)MappedResource.pData;
        memcpy(taaParameters, &m_taaParameters, sizeof(TAAParameters));
        fw::DX::context->Unmap(m_taaParametersBuffer, 0);
    }
}

void TAAApp::render()
{
    ID3D11DeviceContext* context = fw::DX::context;
    context->ClearRenderTargetView(m_currentFrameRtv, clearColor);
    context->ClearRenderTargetView(m_motionRtv, clearMotion);
    context->ClearDepthStencilView(m_depthmapDsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Render
    ID3D11RenderTargetView* rtvs[] = {m_currentFrameRtv, m_motionRtv};
    context->OMSetRenderTargets(2, rtvs, m_depthmapDsv);
    fw::VertexBuffer* vb = m_vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    context->IASetInputLayout(m_renderVS.getVertexLayout());
    context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_renderVS.get(), nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
    context->VSSetConstantBuffers(1, 1, &m_prevMatrixBuffer);
    context->PSSetShader(m_renderPS.get(), nullptr, 0);
    context->PSSetSamplers(0, 1, &m_linearSampler);
    context->PSSetShaderResources(0, 1, &m_textureView);
    context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);

    // TAA
    context->OMSetRenderTargets(1, &m_outputFrameRtv, nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(nullptr);
    context->VSSetShader(m_taaVS.get(), nullptr, 0);
    context->PSSetShader(m_taaPS.get(), nullptr, 0);
    context->PSSetSamplers(0, 1, &m_linearSampler);
    context->PSSetShaderResources(0, 1, &m_currentFrameSrv);
    context->PSSetShaderResources(1, 1, &m_prevFrameSrv);
    context->PSSetShaderResources(2, 1, &m_motionSrv);
    context->PSSetConstantBuffers(0, 1, &m_taaParametersBuffer);
    context->Draw(3, 0);

    ID3D11ShaderResourceView* nullSrv[] = {NULL, NULL, NULL};
    context->PSSetShaderResources(0, 3, nullSrv);

    ID3D11RenderTargetView* nullRtv[] = {NULL};
    fw::DX::context->OMSetRenderTargets(1, nullRtv, nullptr);

    // Blit results
    m_blitter.blit(m_outputFrameSrv, m_prevFrameRtv);
    m_blitter.blit(m_outputFrameSrv, fw::DX::renderTargetView);
}

void TAAApp::gui()
{
    const char* format = "%.2f";
    ImGui::SliderFloat("Blend lerp", &m_taaParameters.blendRatio, 0.01f, 0.5f, format);
    ImGui::Checkbox("Disable jitter", &m_disableJitter);

    static int counter = 0;
    static float frameTime = 0.0f;
    static float fps = 0.0f;
    ++counter;
    if (counter > 1000)
    {
        counter = 0;
        frameTime = 1000.0f / ImGui::GetIO().Framerate;
        fps = ImGui::GetIO().Framerate;
    }
    ImGui::Text("%.3f ms/frame (%.1f FPS)", frameTime, fps);
}

bool TAAApp::createConstantBuffers()
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

    hr = fw::DX::device->CreateBuffer(&bd, nullptr, &m_prevMatrixBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create prev matrix buffer", &hr);
        return false;
    }

    bd.ByteWidth = sizeof(TAAParameters);
    hr = fw::DX::device->CreateBuffer(&bd, nullptr, &m_taaParametersBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create TAA parameters buffer", &hr);
        return false;
    }

    return true;
}

bool TAAApp::createFrameTextures()
{
    auto createTexture = [](ID3D11Texture2D*& texture, DXGI_FORMAT format) {
        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = static_cast<UINT>(fw::API::getWindowWidth());
        textureDesc.Height = static_cast<UINT>(fw::API::getWindowHeight());
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = format;
        textureDesc.SampleDesc = DXGI_SAMPLE_DESC{1, 0};
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;
        HRESULT hr = fw::DX::device->CreateTexture2D(&textureDesc, NULL, &texture);

        if (FAILED(hr))
        {
            fw::printError("Failed to create frame texture", &hr);
            return false;
        }
        return true;
    };

    auto createRtv = [](ID3D11Texture2D* texture, ID3D11RenderTargetView*& rtv, DXGI_FORMAT format) {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        HRESULT hr = fw::DX::device->CreateRenderTargetView(texture, &rtvDesc, &rtv);
        if (FAILED(hr))
        {
            fw::printError("Failed to create frame RTV", &hr);
            return false;
        }
        return true;
    };

    auto createSrv = [](ID3D11Texture2D* texture, ID3D11ShaderResourceView*& srv, DXGI_FORMAT format) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        HRESULT hr = fw::DX::device->CreateShaderResourceView(texture, &srvDesc, &srv);
        if (FAILED(hr))
        {
            fw::printError("Failed to create frame SRV", &hr);
            return false;
        }
        return true;
    };

    DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT motionFormat = DXGI_FORMAT_R16G16_FLOAT;

    bool ok = //
        createTexture(m_currentFrameTexture, format) && //
        createTexture(m_prevFrameTexture, format) && //
        createTexture(m_outputFrameTexture, format) && //
        createTexture(m_motionTexture, motionFormat) && //
        createRtv(m_currentFrameTexture, m_currentFrameRtv, format) && //
        createRtv(m_prevFrameTexture, m_prevFrameRtv, format) && //
        createRtv(m_outputFrameTexture, m_outputFrameRtv, format) && //
        createRtv(m_motionTexture, m_motionRtv, motionFormat) && //
        createSrv(m_currentFrameTexture, m_currentFrameSrv, format) && //
        createSrv(m_prevFrameTexture, m_prevFrameSrv, format) && //
        createSrv(m_outputFrameTexture, m_outputFrameSrv, format) && //
        createSrv(m_motionTexture, m_motionSrv, motionFormat);

    if (!ok)
    {
        return false;
    }

    // Depth
    D3D11_TEXTURE2D_DESC depthmapDesc{};
    depthmapDesc.Width = static_cast<UINT>(fw::API::getWindowWidth());
    depthmapDesc.Height = static_cast<UINT>(fw::API::getWindowHeight());
    depthmapDesc.MipLevels = 1;
    depthmapDesc.ArraySize = 1;
    depthmapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthmapDesc.SampleDesc = DXGI_SAMPLE_DESC{1, 0};
    depthmapDesc.Usage = D3D11_USAGE_DEFAULT;
    depthmapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthmapDesc.CPUAccessFlags = 0;
    depthmapDesc.MiscFlags = 0;
    HRESULT hr = fw::DX::device->CreateTexture2D(&depthmapDesc, nullptr, &m_depthmapTexture);
    if (FAILED(hr))
    {
        fw::printError("Failed to create depthmap", &hr);
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    hr = fw::DX::device->CreateDepthStencilView(m_depthmapTexture, &depthStencilViewDesc, &m_depthmapDsv);
    if (FAILED(hr))
    {
        fw::printError("Failed to create dsv", &hr);
        return false;
    }

    return true;
}
