#include "CSMApp.h"
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/imgui/imgui.h>
#include <DirectXMath.h>
#include <vector>
#include <iostream>

namespace
{

float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

} // anonymous

CSMApp::CSMApp()
{
}

CSMApp::~CSMApp()
{
	fw::release(matrixBuffer);	
}

bool CSMApp::initialize()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!vertexShader.create(L"lighting.fx", "VS", "vs_4_0", layout)) {
		return false;
	}

	if (!pixelShader.create(L"lighting.fx", "PS", "ps_4_0")) {
		return false;
	}
	
	if (!createBuffer<MatrixData>(&matrixBuffer)) {
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

	light.transformation.position = DirectX::XMVectorSet(2.0f, 5.0f, 2.0f, 0.0f);
	light.transformation.rotation = DirectX::XMVectorSet(0.8f, -0.8f, 0.0f, 0.0f);

	camera.getTransformation().position = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	camera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
	camera.updateViewMatrix();
	cameraController.setCamera(&camera);
	cameraController.setResetPosition({5.5f, 3.6f, -5.6f});
	cameraController.setResetRotation({0.4f, -0.6f, 0.0f});

	std::cout << "CSMApp initialization completed\n";

	return true;
}

void CSMApp::update()
{
	if (fw::API::isKeyReleased(DirectX::Keyboard::Escape)) {
		fw::API::quit();
	}
	
	cameraController.update();
	camera.updateViewMatrix();
}

void CSMApp::render()
{
	fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
	fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);

	fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);
	fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	fw::DX::context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DirectionalLightData* lightData = (DirectionalLightData*)mappedResource.pData;
	lightData->position = light.transformation.position;
	lightData->direction = light.transformation.getForward();
	lightData->color = light.color;
	fw::DX::context->Unmap(lightBuffer, 0);
	fw::DX::context->PSSetConstantBuffers(1, 1, &lightBuffer);
		
	renderObject(monkey1);
	renderObject(monkey2);
	renderObject(cube);
}

void CSMApp::gui()
{
	fw::displayVector("Camera position %.1f, %.1f, %.1f", camera.getTransformation().position);
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

void CSMApp::renderObject(const RenderData& renderData)
{
	fw::AssetManager::VertexBuffer* vb = renderData.vertexBuffer;
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
