#include "PixelShader.h"
#include "Common.h"
#include "DX.h"
#include <d3dcompiler.h>

namespace fw
{
PixelShader::PixelShader()
{
}

PixelShader::~PixelShader()
{
    release(pixelShader);
}

bool PixelShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel)
{
    ID3DBlob* blob = nullptr;
    bool compiled = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
    if (!compiled)
    {
        return false;
    }

    HRESULT hr = DX::device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);
    blob->Release();

    if (FAILED(hr))
    {
        printError("Failed to create pixel shader", &hr);
        return false;
    }

    return true;
}

ID3D11PixelShader* PixelShader::get() const
{
    return pixelShader;
}

} // namespace fw