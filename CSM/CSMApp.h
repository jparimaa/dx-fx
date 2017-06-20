#pragma once

#include "DataTypes.h"
#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Common.h>
#include <fw/Transformation.h>
#include <fw/PerspectiveCamera.h>
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
	fw::PerspectiveCamera camera;
	fw::CameraController cameraController;
	fw::VertexShader lightingVS;
	fw::PixelShader lightingPS;
	fw::VertexShader depthmapVS;
	fw::AssetManager assetManager;

	ID3D11Buffer* matrixBuffer = nullptr;
	ID3D11Buffer* lightBuffer = nullptr;
	ID3D11SamplerState* samplerLinear = nullptr;

	ID3D11Texture2D* depthmapTexture = nullptr;
	ID3D11DepthStencilView* depthmapDSV = nullptr;
	ID3D11ShaderResourceView* depthmapSRV = nullptr;
	ID3D11SamplerState* samplerShadow = nullptr;

	RenderData cube;
	RenderData monkey1;
	RenderData monkey2;
	DirectionalLight light;

	template <typename T>
	bool createBuffer(ID3D11Buffer** buffer);
	bool createDepthmap();
	void renderDepthmap();
	void renderObjects();
	void renderObject(const RenderData& renderData);
};