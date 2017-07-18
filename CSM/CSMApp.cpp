#include "CSMApp.h"
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/imgui/imgui.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include <iostream>

namespace
{

float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

template <typename T>
std::array<DirectX::XMFLOAT3, 8> getFrustumCorners(const T& camera, float nearClip, float farClip)
{
	T cam = camera;
	cam.setNearClipDistance(nearClip);
	cam.setFarClipDistance(farClip);
	cam.updateViewMatrix();
	cam.updateProjectionMatrix();
	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, cam.getProjectionMatrix());
	DirectX::XMMATRIX inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, cam.getViewMatrix());
	frustum.Transform(frustum, inverseViewMatrix);
	std::array<DirectX::XMFLOAT3, 8> corners;
	frustum.GetCorners(corners.data());
	return corners;
}

} // anonymous

CSMApp::CSMApp()
{
}

CSMApp::~CSMApp()
{
	fw::release(matrixBuffer);
	fw::release(lightMatrixBuffer);
	fw::release(lightBuffer);
	fw::release(depthmapTexture);
	fw::release(depthmapDSV);
	fw::release(depthmapSRV);
}

bool CSMApp::initialize()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> defaultLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!lightingVS.create(L"lighting.fx", "VS", "vs_4_0", defaultLayout)) {
		return false;
	}

	if (!lightingPS.create(L"lighting.fx", "PS", "ps_4_0")) {
		return false;
	}

	if (!depthmapVS.create(L"depthmap.fx", "VS", "vs_4_0", defaultLayout)) {
		return false;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> frustumLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!frustumVS.create(L"frustum.fx", "VS", "vs_4_0", frustumLayout)) {
		return false;
	}

	if (!frustumPS.create(L"frustum.fx", "PS", "ps_4_0")) {
		return false;
	}

	if (!createDepthmap()) {
		return false;
	}

	if (!createBuffer<MatrixData>(&matrixBuffer)) {
		return false;
	}

	if (!createBuffer<LightMatrixData>(&lightMatrixBuffer)) {
		return false;
	}

	if (!createBuffer<DirectionalLightData>(&lightBuffer)) {
		return false;
	}

	if (!assetManager.getLinearSampler(&samplerLinear)) {
		return false;
	}

	monkey1.textureView = assetManager.getTextureView("../Assets/checker.png");
	monkey1.vertexBuffer = assetManager.getVertexBuffer("../Assets/monkey.3ds");
	monkey1.transformation.position = DirectX::XMVectorSet(4.0f, 1.0f, 0.0f, 0.0f);
	monkey1.transformation.updateWorldMatrix();

	monkey2.textureView = assetManager.getTextureView("../Assets/green_square.png");
	monkey2.vertexBuffer = assetManager.getVertexBuffer("../Assets/monkey.3ds");
	monkey2.transformation.position = DirectX::XMVectorSet(-2.0f, 1.0f, 0.0f, 0.0f);
	monkey2.transformation.updateWorldMatrix();

	cube.textureView = assetManager.getTextureView("../Assets/checker.png");
	cube.vertexBuffer = assetManager.getVertexBuffer("../Assets/cube.obj");
	cube.transformation.scale = DirectX::XMVectorSet(10.0f, 0.01f, 10.0f, 0.0f);
	cube.transformation.updateWorldMatrix();

	viewCamera.getTransformation().position = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	viewCamera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
	viewCamera.updateViewMatrix();
	cameraController.setCameraTransformation(&viewCamera.getTransformation());
	cameraController.setResetPosition({6.0f, 6.0f, -6.0f});
	cameraController.setResetRotation({0.8f, -0.8f, 0.0f});
	cameraController.setMovementSpeed(2.0f);

	light.transformation.position = DirectX::XMVectorSet(6.0f, 6.0f, -6.0f, 0.0f);
	light.transformation.rotation = DirectX::XMVectorSet(0.8f, -0.8f, 0.0f, 0.0f);
	lightCamera.getTransformation().position = light.transformation.position;
	lightCamera.getTransformation().rotation = light.transformation.rotation;
	lightCamera.updateViewMatrix();
	lightCamera.updateProjectionMatrix();

	fw::DX::context->VSSetShader(lightingVS.get(), nullptr, 0);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	fw::DX::context->Map(lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightMatrixData* lightMatrixData = (LightMatrixData*)mappedResource.pData;
	lightMatrixData->viewProjection = lightCamera.getViewMatrix() * lightCamera.getProjectionMatrix();
	fw::DX::context->Unmap(lightMatrixBuffer, 0);
	fw::DX::context->VSSetConstantBuffers(1, 1, &lightMatrixBuffer);

	viewCameraFrustumCorners.push_back(getFrustumCorners(viewCamera, viewCamera.getNearClipDistance(), frustumDivisions[0]));
	viewCameraFrustumCorners.push_back(getFrustumCorners(viewCamera, frustumDivisions[0], frustumDivisions[1]));
	viewCameraFrustumCorners.push_back(getFrustumCorners(viewCamera, frustumDivisions[0], viewCamera.getFarClipDistance()));
		
	fw::OrthographicCamera orthoCam;
	orthoCam = getCascadedCamera(viewCamera.getNearClipDistance(), frustumDivisions[0]);
	// This does not work for ortho cameras.
	shadowMapFrustumCorners.push_back(getFrustumCorners(orthoCam, orthoCam.getNearClipDistance(), orthoCam.getFarClipDistance()));
	
	std::cout << "CSMApp initialization completed\n";

	return true;
}

void CSMApp::update()
{
	if (fw::API::isKeyReleased(DirectX::Keyboard::Escape)) {
		fw::API::quit();
	}

	monkey1.transformation.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.1f);
	monkey1.transformation.updateWorldMatrix();

	cameraController.update();
	viewCamera.updateViewMatrix();
}

void CSMApp::render()
{
	renderDepthmap();
	renderObjects();

	drawFrustumLines(viewCameraFrustumCorners[0]);
	drawFrustumLines(viewCameraFrustumCorners[1]);
	drawFrustumLines(viewCameraFrustumCorners[2]);

	drawFrustumLines(shadowMapFrustumCorners[0]);

	ID3D11ShaderResourceView* nv[] = {nullptr};
	fw::DX::context->PSSetShaderResources(1, 1, nv);
}

void CSMApp::gui()
{
	fw::displayVector("Camera position %.1f, %.1f, %.1f", viewCamera.getTransformation().position);
	/*
	fw::displayVector("Camera rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
	fw::displayVector("Camera direction %.1f %.1f %.1f", camera.getTransformation().getForward());
	fw::displayVector("Light direction %.1f %.1f %.1f", light.transformation.getForward());
	ImGui::ColorEdit3("Light color", light.color.data());
	*/
	ImGui::DragFloat("Light power", &light.color[3], 0.01f);
}

template<typename T>
bool CSMApp::createBuffer(ID3D11Buffer** buffer)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(T);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, nullptr, buffer);
	if (FAILED(hr)) {
		fw::printError("Failed to create buffer", &hr);
	}
	return true;
}

bool CSMApp::createDepthmap()
{
	D3D11_TEXTURE2D_DESC shadowmapDesc = {
		fw::API::getWindowWidth(),
		fw::API::getWindowHeight(),
		1, // Miplevels
		1, // ArraySize
		DXGI_FORMAT_R32_TYPELESS,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, // Bind flags
		0, // CPU access flag
		0 // Misc flags
	};
	fw::DX::device->CreateTexture2D(&shadowmapDesc, nullptr, &depthmapTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilVD = {
		DXGI_FORMAT_D32_FLOAT,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};
	fw::DX::device->CreateDepthStencilView(depthmapTexture, &depthStencilVD, &depthmapDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceVD = {
		DXGI_FORMAT_R32_FLOAT,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	shaderResourceVD.Texture2D.MipLevels = 1;
	fw::DX::device->CreateShaderResourceView(depthmapTexture, &shaderResourceVD, &depthmapSRV);

	return true;
}

void CSMApp::renderDepthmap()
{
	fw::DX::context->ClearDepthStencilView(depthmapDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
	ID3D11RenderTargetView* nullView = nullptr;
	// Set a null render target to not render color.
	fw::DX::context->OMSetRenderTargets(1, &nullView, depthmapDSV);
	fw::DX::context->IASetInputLayout(depthmapVS.getVertexLayout());
	fw::DX::context->VSSetShader(depthmapVS.get(), nullptr, 0);
	fw::DX::context->PSSetShader(nullptr, nullptr, 0);

	renderObject(monkey1, lightCamera);
	renderObject(monkey2, lightCamera);
	renderObject(cube, lightCamera);

	fw::DX::context->OMSetRenderTargets(1, &nullView, nullptr);
}

void CSMApp::renderObjects()
{
	ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
	fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());
	fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
	fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	fw::DX::context->IASetInputLayout(lightingVS.getVertexLayout());
	fw::DX::context->VSSetShader(lightingVS.get(), nullptr, 0);

	fw::DX::context->PSSetShader(lightingPS.get(), nullptr, 0);
	fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);
	fw::DX::context->PSSetShaderResources(1, 1, &depthmapSRV);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	fw::DX::context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DirectionalLightData* lightData = (DirectionalLightData*)mappedResource.pData;
	lightData->position = light.transformation.position;
	lightData->direction = light.transformation.getForward();
	lightData->color = light.color;
	fw::DX::context->Unmap(lightBuffer, 0);
	fw::DX::context->PSSetConstantBuffers(2, 1, &lightBuffer);

	renderObject(monkey1, viewCamera);
	renderObject(monkey2, viewCamera);
	renderObject(cube, viewCamera);
}

void CSMApp::renderObject(const RenderData& renderData, const fw::Camera& camera)
{
	fw::VertexBuffer* vb = renderData.vertexBuffer;
	fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
	fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	fw::DX::context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixData* matrixData = (MatrixData*)mappedResource.pData;
	matrixData->world = renderData.transformation.getWorldMatrix();
	matrixData->view = camera.getViewMatrix();
	matrixData->projection = camera.getProjectionMatrix();
	fw::DX::context->Unmap(matrixBuffer, 0);
	fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);

	fw::DX::context->PSSetShaderResources(0, 1, &renderData.textureView);
	fw::DX::context->DrawIndexed(vb->numIndices, 0, 0);
}

fw::OrthographicCamera CSMApp::getCascadedCamera(float nearPlane, float farPlane)
{
	fw::PerspectiveCamera frustumCamera = viewCamera;
	frustumCamera.setNearClipDistance(nearPlane);
	frustumCamera.setFarClipDistance(farPlane);
	frustumCamera.updateProjectionMatrix();

	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, frustumCamera.getProjectionMatrix());
	DirectX::XMMATRIX inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, frustumCamera.getViewMatrix());	
	frustum.Transform(frustum, inverseViewMatrix);
	frustum.Transform(frustum, lightCamera.getViewMatrix());
	
	std::array<DirectX::XMFLOAT3, 8> corners;	
	frustum.GetCorners(corners.data());
	DirectX::BoundingBox boundingBox;
	DirectX::BoundingBox::CreateFromPoints(boundingBox, corners.size(), corners.data(), sizeof(DirectX::XMFLOAT3));
	
	fw::OrthographicCamera cascadeCamera = lightCamera;
	float x = boundingBox.Extents.x;
	float y = boundingBox.Extents.y;
	float farClipPlane = 0.0f;
	for (int i = 0; i < 8; ++i) {
		farClipPlane = max(farClipPlane, corners[i].z);
	}
	cascadeCamera.setViewBox(-x, x, -y, y, 0.001f, farClipPlane);
	
	return cascadeCamera;
}

// This is slow, use only for debug purposes.
void CSMApp::drawFrustumLines(const std::array<DirectX::XMFLOAT3, 8>& points)
{
	fw::VertexBuffer buffer;
	buffer.releaseOnDestruction = true;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DirectX::XMFLOAT3) * points.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = points.data();

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &buffer.vertexBuffer);
	if (FAILED(hr)) {
		fw::printError("ERROR: Failed to create vertex buffer in frustum line drawing", &hr);
		return;
	}

	std::vector<WORD> indices{0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 5, 1, 2, 6, 7, 3};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	data.pSysMem = indices.data();

	hr = fw::DX::device->CreateBuffer(&bd, &data, &buffer.indexBuffer);
	if (FAILED(hr)) {
		fw::printError("ERROR: Failed to create index buffer", &hr);
		return;
	}

	buffer.stride = sizeof(DirectX::XMFLOAT3);
	ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
	fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());
	fw::DX::context->IASetInputLayout(frustumVS.getVertexLayout());
	fw::DX::context->VSSetShader(frustumVS.get(), nullptr, 0);
	fw::DX::context->PSSetShader(frustumPS.get(), nullptr, 0);
	fw::DX::context->IASetVertexBuffers(0, 1, &buffer.vertexBuffer, &buffer.stride, &buffer.offset);
	fw::DX::context->IASetIndexBuffer(buffer.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	fw::DX::context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	fw::DX::context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixData* matrixData = (MatrixData*)mappedResource.pData;
	matrixData->world = DirectX::XMMATRIX();
	matrixData->view = viewCamera.getViewMatrix();
	matrixData->projection = viewCamera.getProjectionMatrix();
	fw::DX::context->Unmap(matrixBuffer, 0);
	fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);

	fw::DX::context->DrawIndexed(indices.size(), 0, 0);
}
