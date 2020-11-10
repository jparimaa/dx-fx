#include "DomainShader.h"
#include "Common.h"
#include "DX.h"
#include <d3dcompiler.h>

namespace fw
{
DomainShader::DomainShader()
{
}

DomainShader::~DomainShader()
{
    release(domainShader);
}

bool DomainShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel)
{
    ID3DBlob* blob = nullptr;
    bool compiled = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
    if (!compiled)
    {
        return false;
    }

    HRESULT hr = DX::device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &domainShader);
    blob->Release();

    if (FAILED(hr))
    {
        printError("Failed to create domain shader", &hr);
        return false;
    }

    return true;
}

ID3D11DomainShader* DomainShader::get() const
{
    return domainShader;
}

} // namespace fw