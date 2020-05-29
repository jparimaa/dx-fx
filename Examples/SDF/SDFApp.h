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
    fw::PerspectiveCamera m_camera;
    fw::CameraController m_cameraController;
    fw::VertexShader m_vertexShader;

    ShaderReloader<ID3D11PixelShader> m_shaderReloader;
};