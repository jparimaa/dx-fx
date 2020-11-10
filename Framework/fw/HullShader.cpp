#include "HullShader.h"
#include "Common.h"
#include "DX.h"
#include <d3dcompiler.h>

namespace fw
{
HullShader::HullShader()
{
}

HullShader::~HullShader()
{
    release(hullShader);
}

bool HullShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel)
{
    ID3DBlob* blob = nullptr;
    bool compiled = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
    if (!compiled)
    {
        return false;
    }

    HRESULT hr = DX::device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &hullShader);
    blob->Release();

    if (FAILED(hr))
    {
        printError("Failed to create hull shader", &hr);
        return false;
    }

    return true;
}

ID3D11HullShader* HullShader::get() const
{
    return hullShader;
}

} // namespace fw