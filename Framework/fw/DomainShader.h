#pragma once

#include <d3d11.h>

namespace fw
{
class DomainShader
{
public:
    DomainShader();
    ~DomainShader();

    bool create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel);
    ID3D11DomainShader* get() const;

private:
    ID3D11DomainShader* domainShader = nullptr;
};

} // namespace fw
