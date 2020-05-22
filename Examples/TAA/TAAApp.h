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
#include <memory>

class TAAApp : public fw::Application
{
public:
    TAAApp();
    virtual ~TAAApp();

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

    fw::PerspectiveCamera m_camera;
    fw::CameraController m_cameraController;
    fw::Transformation m_transformation;
    fw::VertexShader m_vertexShader;
    fw::PixelShader m_pixelShader;
    fw::AssetManager m_assetManager;
    fw::VertexBuffer* m_vertexBuffer = nullptr;

    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11ShaderResourceView* m_textureView = nullptr;
    ID3D11SamplerState* m_samplerLinear = nullptr;

    bool createMatrixBuffer();
};