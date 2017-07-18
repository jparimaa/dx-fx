#pragma once

#include "DataTypes.h"
#include <fw/Application.h>
#include <fw/VertexShader.h>
#include <fw/PixelShader.h>
#include <fw/Common.h>
#include <fw/Transformation.h>
#include <fw/Camera.h>
#include <fw/PerspectiveCamera.h>
#include <fw/OrthographicCamera.h>
#include <fw/CameraController.h>
#include <fw/AssetManager.h>
#include <d3d11.h>
#include <memory>
#include <array>

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
	fw::PerspectiveCamera viewCamera;
	fw::OrthographicCamera lightCamera;
	fw::CameraController cameraController;
	fw::VertexShader lightingVS;
	fw::PixelShader lightingPS;
	fw::VertexShader depthmapVS;
	fw::VertexShader frustumVS;
	fw::PixelShader frustumPS;
	fw::AssetManager assetManager;
	
	ID3D11Buffer* matrixBuffer = nullptr;
	ID3D11Buffer* lightMatrixBuffer = nullptr;
	ID3D11Buffer* lightBuffer = nullptr;
	ID3D11SamplerState* samplerLinear = nullptr;

	ID3D11Texture2D* depthmapTexture = nullptr;
	ID3D11DepthStencilView* depthmapDSV = nullptr;
	ID3D11ShaderResourceView* depthmapSRV = nullptr;

	RenderData cube;
	RenderData monkey1;
	RenderData monkey2;
	DirectionalLight light;
	
	std::array<float, 2> frustumDivisions = {5.0f, 15.0f};
	std::array<fw::OrthographicCamera, 3> cascadeCameras;

	using FrustumCorners = std::array<DirectX::XMFLOAT3, 8>;
	std::vector<FrustumCorners> viewCameraFrustumCorners;
	std::vector<FrustumCorners> shadowMapFrustumCorners;

	template <typename T>
	bool createBuffer(ID3D11Buffer** buffer);
	bool createDepthmap();
	void renderDepthmap();
	void renderObjects();
	void renderObject(const RenderData& renderData, const fw::Camera& camera);
	fw::OrthographicCamera getCascadedCamera(float nearPlane, float farPlane);
	void drawFrustumLines(const std::array<DirectX::XMFLOAT3, 8>& points);
};