#pragma once

#include "DataTypes.h"
#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Common.h>
#include <fw/Transformation.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/AssetManager.h>
#include <d3d11.h>
#include <memory>

class CSMApp : public fw::Application
{
public:
	CSMApp();
	virtual ~CSMApp();

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Camera camera;
	fw::CameraController cameraController;
	fw::VertexShader vertexShader;
	fw::PixelShader pixelShader;
	fw::AssetManager assetManager;

	ID3D11Buffer* matrixBuffer = nullptr;
	ID3D11Buffer* lightBuffer = nullptr;
	ID3D11SamplerState* samplerLinear = nullptr;

	RenderData cube;
	RenderData monkey1;
	RenderData monkey2;
	DirectionalLight light;

	bool createMatrixBuffer();
	bool createLightBuffer();
	void renderObject(const RenderData& renderData);
};