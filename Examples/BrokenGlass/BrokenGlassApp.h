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

    ID3D11Texture2D* depthmapTexture = nullptr;
    ID3D11DepthStencilView* depthmapDSV = nullptr;
    ID3D11ShaderResourceView* depthmapSRV = nullptr;

    ID3D11Texture2D* sceneInputTexture = nullptr;
    ID3D11RenderTargetView* sceneInputRTV = nullptr;
    ID3D11ShaderResourceView* sceneInputSRV = nullptr;

    ID3D11Texture2D* outputTexture = nullptr;
    ID3D11RenderTargetView* outputRTV = nullptr;
    ID3D11ShaderResourceView* outputSRV = nullptr;

    bool createMatrixBuffer(ID3D11Buffer*& matrixBuffer);
    void updateMatrixBuffer(const TransformData& td);
    bool createDepthMap();
    bool createRenderTarget(ID3D11Texture2D*& sceneInputTexture, ID3D11RenderTargetView*& sceneInputView, ID3D11ShaderResourceView*& sceneInputSRV);
};