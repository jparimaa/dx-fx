#pragma once

#include "ShaderReloader.h"

#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/PerspectiveCamera.h>
#include <fw/CameraController.h>
#include <fw/Common.h>

class SDFApp : public fw::Application
{
public:
    SDFApp(){};
    virtual ~SDFApp(){};

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    struct Constants
    {
        float time;
        float padding[3];
        DirectX::XMVECTOR cameraPos;
        DirectX::XMVECTOR cameraDir;
        DirectX::XMMATRIX cameraTransform;
        DirectX::XMMATRIX sphere1Transform;
        DirectX::XMMATRIX sphere2Transform;
        DirectX::XMMATRIX sphere3Transform;
        DirectX::XMMATRIX sphereBoxTransform;
    };

    fw::PerspectiveCamera m_camera;
    fw::CameraController m_cameraController;
    fw::VertexShader m_vertexShader;

    fw::Transformation m_sphere1Transform;
    fw::Transformation m_sphere2Transform;
    fw::Transformation m_sphere3Transform;
    fw::Transformation m_sphereBoxTransform;
    ID3D11Buffer* m_constantBuffer = nullptr;
    Constants m_constants;

    ShaderReloader<ID3D11PixelShader> m_shaderReloader;

    bool createConstantBuffer();
};