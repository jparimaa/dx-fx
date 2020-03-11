#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Transformation.h>
#include <fw/PerspectiveCamera.h>
#include <fw/CameraController.h>
#include <fw/AssetManager.h>
#include <fw/Common.h>

#include <DirectXMath.h>
#include <d3d11.h>

class BrokenGlassApp : public fw::Application
{
public:
    struct ShaderProgram
    {
        fw::VertexShader vertexShader;
        fw::PixelShader pixelShader;
    };

    BrokenGlassApp();
    virtual ~BrokenGlassApp();

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    struct MatrixData
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    struct TransformData
    {
        fw::Transformation transform;
        ID3D11Buffer* matrixBuffer = nullptr;

        ~TransformData()
        {
            fw::release(matrixBuffer);
        }
    };

    fw::PerspectiveCamera camera;
    fw::CameraController cameraController;
    TransformData monkeyData;
    TransformData cubeData;
    ShaderProgram diffuseShader;
    ShaderProgram warpedShader;
    fw::AssetManager assetManager;
    fw::VertexBuffer* monkeyVertexBuffer = nullptr;
    fw::VertexBuffer* cubeVertexBuffer = nullptr;
    D3D11_VIEWPORT viewport;

    ID3D11ShaderResourceView* diffuseTextureView = nullptr;
    ID3D11ShaderResourceView* glassTextureView = nullptr;
    ID3D11SamplerState* samplerLinear = nullptr;

    ID3D11Texture2D* renderTargetTexture = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11ShaderResourceView* renderTargetSRV = nullptr;

    bool createMatrixBuffer(ID3D11Buffer*& matrixBuffer);
    void updateMatrixBuffer(const TransformData& td);
    bool createRenderTarget();
};