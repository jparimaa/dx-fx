#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/AssetManager.h>
#include <fw/Common.h>
#include <d3d11.h>

class FullscreenApp : public fw::Application
{
public:
    FullscreenApp();
    virtual ~FullscreenApp();

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::VertexShader vertexShader;
    fw::PixelShader pixelShader;
    fw::AssetManager assetManager;

    ID3D11ShaderResourceView* textureView = nullptr;
    ID3D11SamplerState* samplerLinear = nullptr;
};