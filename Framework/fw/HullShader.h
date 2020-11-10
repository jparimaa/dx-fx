#pragma once

#include <d3d11.h>

namespace fw
{
class HullShader
{
public:
    HullShader();
    ~HullShader();

    bool create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel);
    ID3D11HullShader* get() const;

private:
    ID3D11HullShader* hullShader = nullptr;
};

} // namespace fw
