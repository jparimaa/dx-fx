#pragma once

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/HullShader.h>
#include <fw/DomainShader.h>
#include <fw/Transformation.h>
#include <fw/PerspectiveCamera.h>
#include <fw/CameraController.h>
#include <fw/Common.h>

#include <DirectXMath.h>
#include <d3d11.h>

#include <memory>

class VolumetricExplosionApp : public fw::Application
{
public:
    VolumetricExplosionApp();
    virtual ~VolumetricExplosionApp();

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
    fw::HullShader m_hullShader;
    fw::DomainShader m_domainShader;
    fw::PixelShader m_pixelShader;

    ID3D11Texture3D* m_noiseVolume = nullptr;
    ID3D11ShaderResourceView* m_noiseVolumeSRV = nullptr;
    ID3D11ShaderResourceView* m_gradientSRV = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11SamplerState* m_samplerClamped = nullptr;
    ID3D11SamplerState* m_samplerWrapped = nullptr;

    uint16_t m_maxNoiseValue = 0;
    uint16_t m_minNoiseValue = 0xFFFF;

    bool createNoiseTexture();
    bool createGradientTexture();
    bool createSamplers();
    bool createMatrixBuffer();
};
