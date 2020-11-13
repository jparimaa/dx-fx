#include "VolumetricExplosionApp.h"
#include <fw/Common.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>

#include <DDSTextureLoader.h>

#include <fstream>
#include <vector>
#include <iostream>

namespace
{
const float c_clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};
} // namespace

VolumetricExplosionApp::VolumetricExplosionApp()
{
}

VolumetricExplosionApp::~VolumetricExplosionApp()
{
    fw::release(m_noiseVolume);
    fw::release(m_matrixBuffer);
}

bool VolumetricExplosionApp::initialize()
{
    const std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    const std::string shaderFile = ROOT_PATH + std::string("Examples/VolumetricExplosion/example.hlsl");
    fw::ToWchar wcharHelper(shaderFile);
    if (!m_vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0", layout))
    {
        return false;
    }

    if (!m_pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }

    bool resourcesCreated = //
        createNoiseTexture() && //
        createGradientTexture() && //
        createSamplers() && //
        createConstantBuffer();

    if (!resourcesCreated)
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

    updateConstantBuffer();
}

void VolumetricExplosionApp::render()
{
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, c_clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    fw::DX::context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    fw::DX::context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    //fw::DX::context->PSSetSamplers(0, 1, &m_linearSampler);

    fw::DX::context->IASetInputLayout(m_vertexShader.getVertexLayout());
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    fw::DX::context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
}

void VolumetricExplosionApp::gui()
{
    ImGui::Text("Hello, world!");
}

bool VolumetricExplosionApp::createNoiseTexture()
{
    const uint16_t noiseValueSize = 32;
    const uint16_t noiseValueCount = noiseValueSize * noiseValueSize * noiseValueSize;
    uint16_t noiseValues[noiseValueCount] = {0};
    std::fstream f;
    f.open("noise_32x32x32.dat", std::ios::in);
    if (!f.is_open())
    {
        return false;
    }

    for (uint16_t i = 0; i < noiseValueCount; ++i)
    {
        uint16_t noiseValue;
        f >> noiseValue;

        m_maxNoiseValue = std::max(m_maxNoiseValue, noiseValue);
        m_minNoiseValue = std::min(m_minNoiseValue, noiseValue);

        noiseValues[i] = noiseValue;
    }
    f.close();

    D3D11_TEXTURE3D_DESC texDesc{};
    texDesc.CPUAccessFlags = 0;
    texDesc.Height = noiseValueSize;
    texDesc.Width = noiseValueSize;
    texDesc.Depth = noiseValueSize;
    texDesc.Format = DXGI_FORMAT_R16_FLOAT;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = noiseValues;
    initialData.SysMemPitch = noiseValueSize * 2;
    initialData.SysMemSlicePitch = noiseValueSize * noiseValueSize * 2;

    HRESULT hr = fw::DX::device->CreateTexture3D(&texDesc, &initialData, &m_noiseVolume);
    if (FAILED(hr))
    {
        fw::printError("Failed to create noise volume", &hr);
        return false;
    }

    hr = fw::DX::device->CreateShaderResourceView(m_noiseVolume, nullptr, &m_noiseVolumeSRV);
    if (FAILED(hr))
    {
        fw::printError("Failed to create noise volume SRV", &hr);
        return false;
    }

    return true;
}

bool VolumetricExplosionApp::createGradientTexture()
{
    HRESULT hr = DirectX::CreateDDSTextureFromFile(fw::DX::device, L"gradient.dds", nullptr, &m_gradientSRV);
    if (FAILED(hr))
    {
        fw::printError("Failed to create gradient SRV", &hr);
        return false;
    }

    return true;
}

bool VolumetricExplosionApp::createSamplers()
{
    D3D11_SAMPLER_DESC desc{};
    desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = fw::DX::device->CreateSamplerState(&desc, &m_samplerClamped);
    if (FAILED(hr))
    {
        return false;
        fw::printError("Failed to create sampler", &hr);
    }

    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hr = fw::DX::device->CreateSamplerState(&desc, &m_samplerWrapped);
    if (FAILED(hr))
    {
        return false;
        fw::printError("Failed to create sampler", &hr);
    }

    return true;
}

bool VolumetricExplosionApp::createConstantBuffer()
{
    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof(CBData);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = fw::DX::device->CreateBuffer(&desc, nullptr, &m_matrixBuffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create matrix buffer", &hr);
        return false;
    }
    return true;
}

void VolumetricExplosionApp::updateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    fw::DX::context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CBData* cbData = (CBData*)mappedResource.pData;
    //cbData->world = m_transformation.getWorldMatrix();
    fw::DX::context->Unmap(m_matrixBuffer, 0);
}
