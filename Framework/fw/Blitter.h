#pragma once

#include "VertexShader.h"
#include "PixelShader.h"

#include <d3d11.h>

namespace fw
{
class Blitter
{
public:
    Blitter();
    virtual ~Blitter();

    void blit(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst);

private:
    ID3D11Buffer* vertexBuffer = nullptr;

    fw::VertexShader vertexShader;
    fw::PixelShader pixelShader;

    ID3D11SamplerState* samplerLinear = nullptr;
};
} // namespace fw
