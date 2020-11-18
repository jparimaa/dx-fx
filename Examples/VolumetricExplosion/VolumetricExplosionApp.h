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
#include <wrl.h>

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
    struct CBData
    {
        DirectX::XMMATRIX worldToViewMatrix;
        DirectX::XMMATRIX worldToProjectionMatrix;
        DirectX::XMMATRIX viewToWorldMatrix;

        DirectX::XMFLOAT3 eyePositionWS;
        DirectX::XMFLOAT3 eyeForwardWS;

        float noiseAmplitudeFactor;
        float noiseScale;

        DirectX::XMFLOAT3 explosionPositionWS;
        float explosionRadiusWS;

        DirectX::XMFLOAT3 noiseAnimationSpeed;
        float time;

        float edgeSoftness;
        float noiseFrequencyFactor;
        uint32_t primitiveIdx;
        float opacity;

        float displacementWS;
        float stepSizeWS;
        uint32_t maxNumSteps;
        float noiseInitialAmplitude;

        DirectX::XMFLOAT2 uvScaleBias;
        float invMaxNoiseDisplacement;
        uint32_t numOctaves;

        float skinThickness;
        uint32_t numHullOctaves;
        uint32_t numHullSteps;
        float tessellationFactor;
    };

    fw::PerspectiveCamera m_camera;
    fw::CameraController m_cameraController;
    fw::Transformation m_transformation;
    fw::VertexShader m_vertexShader;
    fw::HullShader m_hullShader;
    fw::DomainShader m_domainShader;
    fw::PixelShader m_pixelShader;

    Microsoft::WRL::ComPtr<ID3D11Texture3D> m_noiseVolume = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_noiseVolumeSRV = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_gradientSRV = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerClamped = nullptr;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerWrapped = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState = nullptr;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState = nullptr;

    uint16_t m_maxNoiseValue = 0;
    uint16_t m_minNoiseValue = 0xFFFF;
    float m_maxSkinThickness = 0.0f;
    float m_maxNoiseDisplacement = 0.0f;
    DirectX::XMMATRIX m_invProjMatrix;

    bool createNoiseTexture();
    void calculateConstantBufferValues();
    bool createGradientTexture();
    bool createSamplers();
    bool createConstantBuffer();
    bool createBlendAndDepthState();
    void updateConstantBuffer();
};
