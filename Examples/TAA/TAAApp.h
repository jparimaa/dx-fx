#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Transformation.h>
#include <fw/PerspectiveCamera.h>
#include <fw/CameraController.h>
#include <fw/AssetManager.h>
#include <fw/Common.h>
#include <fw/Blitter.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <memory>

class TAAApp : public fw::Application
{
public:
    TAAApp(){};
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
        DirectX::XMMATRIX jitter;
    };

    struct TAAParameters
    {
        float blendRatio = 0.01f;
        float padding[3];
    };

    fw::PerspectiveCamera m_camera;
    fw::CameraController m_cameraController;
    fw::Transformation m_transformation;
    fw::VertexShader m_renderVS;
    fw::PixelShader m_renderPS;
    fw::VertexShader m_taaVS;
    fw::PixelShader m_taaPS;
    fw::AssetManager m_assetManager;
    fw::VertexBuffer* m_vertexBuffer = nullptr;

    TAAParameters m_taaParameters;

    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_prevMatrixBuffer = nullptr;
    ID3D11Buffer* m_taaParametersBuffer = nullptr;

    ID3D11ShaderResourceView* m_textureView = nullptr;
    ID3D11SamplerState* m_linearSampler = nullptr;

    ID3D11Texture2D* m_currentFrameTexture = nullptr;
    ID3D11RenderTargetView* m_currentFrameRtv = nullptr;
    ID3D11ShaderResourceView* m_currentFrameSrv = nullptr;

    ID3D11Texture2D* m_depthmapTexture = nullptr;
    ID3D11DepthStencilView* m_depthmapDsv = nullptr;

    ID3D11Texture2D* m_prevFrameTexture = nullptr;
    ID3D11RenderTargetView* m_prevFrameRtv = nullptr;
    ID3D11ShaderResourceView* m_prevFrameSrv = nullptr;

    ID3D11Texture2D* m_outputFrameTexture = nullptr;
    ID3D11RenderTargetView* m_outputFrameRtv = nullptr;
    ID3D11ShaderResourceView* m_outputFrameSrv = nullptr;

    ID3D11Texture2D* m_motionTexture = nullptr;
    ID3D11RenderTargetView* m_motionRtv = nullptr;
    ID3D11ShaderResourceView* m_motionSrv = nullptr;

    int m_haltonIndex = 0;
    float m_jitterX = 0.0f;
    float m_jitterY = 0.0f;

    bool m_disableJitter = false;

    fw::Blitter m_blitter;

    bool createConstantBuffers();
    bool createFrameTextures();
};