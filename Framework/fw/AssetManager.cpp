#include "AssetManager.h"
#include "DX.h"
#include "Common.h"
#include "Model.h"
#include "WcharHelper.h"
#include <WICTextureLoader.h>
#include <iostream>
#include <locale>
#include <codecvt>
#include <sstream>

namespace fw
{
AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
    for (auto& kv : textures)
    {
        release(kv.second.texture);
        release(kv.second.textureView);
    }

    for (auto& kv : modelBuffers)
    {
        release(kv.second.vertexBuffer);
        release(kv.second.indexBuffer);
    }

    fw::release(samplerLinear);
}

ID3D11ShaderResourceView* AssetManager::getTextureView(const std::string& filename)
{
    auto find = textures.find(filename);
    if (find != textures.end())
    {
        return find->second.textureView;
    }

    if (!fw::fileExists(filename.c_str()))
    {
        std::stringstream ss;
        ss << "File not found: " << filename << "\n";
        printError(ss.str());
        return nullptr;
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring ws = converter.from_bytes(filename);
    ID3D11Resource* texture = nullptr;
    ID3D11ShaderResourceView* textureView = nullptr;
    HRESULT hr = DirectX::CreateWICTextureFromFile(fw::DX::device, fw::DX::context, ws.c_str(), &texture, &textureView);
    if (FAILED(hr))
    {
        printError("Failed to create WIC texture from file", &hr);
        return nullptr;
    }

    TextureData td;
    td.texture = texture;
    td.textureView = textureView;
    textures[filename] = td;
    return textureView;
}

VertexBuffer* AssetManager::getVertexBuffer(const std::string& filename)
{
    auto find = modelBuffers.find(filename);
    if (find != modelBuffers.end())
    {
        return &find->second;
    }

    if (!fw::fileExists(filename.c_str()))
    {
        std::stringstream ss;
        ss << "File not found: " << filename << "\n";
        printError(ss.str());
        return nullptr;
    }

    fw::Model model;
    model.loadModel(filename);
    std::vector<float> vertexData = fw::getVertexData(model);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(float) * vertexData.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = vertexData.data();

    VertexBuffer buffer;
    HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &buffer.vertexBuffer);
    if (FAILED(hr))
    {
        printError("ERROR: Failed to create vertex buffer", &hr);
        return nullptr;
    }

    buffer.numIndices = model.getNumIndices();
    std::vector<WORD> indices;
    for (const auto& mesh : model.getMeshes())
    {
        indices.insert(indices.end(), mesh.indices.begin(), mesh.indices.end());
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(WORD) * model.getNumIndices());
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    data.pSysMem = indices.data();

    hr = fw::DX::device->CreateBuffer(&bd, &data, &buffer.indexBuffer);
    if (FAILED(hr))
    {
        printError("ERROR: Failed to create index buffer", &hr);
        return nullptr;
    }

    modelBuffers[filename] = buffer;
    return &modelBuffers[filename];
}

bool AssetManager::getLinearSampler(ID3D11SamplerState** sampler)
{
    if (samplerLinear)
    {
        *sampler = samplerLinear;
    }
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = fw::DX::device->CreateSamplerState(&sampDesc, &samplerLinear);
    if (FAILED(hr))
    {
        printError("Failed to create sampler state", &hr);
        return false;
    }
    *sampler = samplerLinear;
    return true;
}

bool AssetManager::getPointSampler(ID3D11SamplerState** sampler)
{
    if (samplerPoint)
    {
        *sampler = samplerPoint;
    }
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = fw::DX::device->CreateSamplerState(&sampDesc, &samplerPoint);
    if (FAILED(hr))
    {
        printError("Failed to create sampler state", &hr);
        return false;
    }
    *sampler = samplerPoint;
    return true;
}

} // namespace fw