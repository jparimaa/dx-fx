#include "Blitter.h"

#include "DX.h"
#include "Common.h"
#include "WcharHelper.h"
#include "AssetManager.h"

#include <vector>

namespace
{
// clang-format off
const std::vector<float> vertexData{
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
     3.0f,  1.0f, 0.0f, 2.0f, 0.0f,
    -1.0f, -3.0f, 0.0f, 0.0f, 2.0f
};
// clang-format on

const UINT stride = 20;
const UINT offset = 0;
} // namespace

namespace fw
{
Blitter::Blitter()
{
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(float) * vertexData.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = vertexData.data();

    HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &vertexBuffer);
    if (FAILED(hr))
    {
        printError("Failed to create vertex buffer", &hr);
    }

    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}};
    std::string shaderFile = ROOT_PATH + std::string("Framework/shaders/blitter.hlsl");
    ToWchar wcharHelper(shaderFile);
    if (!vertexShader.create(wcharHelper.getWchar(), "VS", "vs_4_0", layout))
    {
        printError("Failed to create blitter vertex shader");
    }

    if (!pixelShader.create(wcharHelper.getWchar(), "PS", "ps_4_0"))
    {
        printError("Failed to create blitter pixel shader");
    }

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = fw::DX::device->CreateSamplerState(&sampDesc, &pointSampler);
    if (FAILED(hr))
    {
        printError("Failed to create sampler state", &hr);
    }
}

Blitter::~Blitter()
{
    fw::release(pointSampler);
    fw::release(vertexBuffer);
}

void Blitter::blit(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst)
{
    fw::DX::context->IASetInputLayout(vertexShader.getVertexLayout());
    fw::DX::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
    fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &pointSampler);
    fw::DX::context->PSSetShaderResources(0, 1, &src);
    fw::DX::context->OMSetRenderTargets(1, &dst, NULL);
    fw::DX::context->Draw(3, 0);

    ID3D11ShaderResourceView* const nullSrv[1] = {NULL};
    fw::DX::context->PSSetShaderResources(0, 1, nullSrv);
}
} // namespace fw