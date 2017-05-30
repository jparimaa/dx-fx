#include "CSMApp.h"
#include <fw/Model.h>
#include <fw/DX.h>
#include <fw/API.h>
#include <fw/imgui/imgui.h>
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <vector>
#include <iostream>

using namespace DirectX;

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
	fw::release(texture);
	fw::release(textureView);
	fw::release(samplerLinear);
}

bool CSMApp::initialize()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!vertexShader.create(L"example.fx", "VS", "vs_4_0", layout)) {
		return false;
	}

	if (!pixelShader.create(L"example.fx", "PS", "ps_4_0")) {
		return false;
	}

	if (!createMatrixBuffer()) {
		return false;
	}

	if (!fw::getLinearSampler(&samplerLinear)) {
		return false;
	}

	HRESULT hr = DirectX::CreateWICTextureFromFile(fw::DX::device, L"../Assets/checker.png", &texture, &textureView);
	if (FAILED(hr)) {
		std::cerr << "ERRROR: Failed to create WIC texture from file\n";
		return false;
	}

	if (!createVertexBuffers(monkeyBuffers, "../Assets/monkey.3ds")) {
		return false;
	}
	
	if (!createVertexBuffers(cubeBuffers, "../Assets/cube.obj")) {
		return false;
	}
	
	monkey1.vertexBuffers = &monkeyBuffers;
	monkey1.transformation.position = XMVectorSet(3.0f, 1.0f, 0.0f, 0.0f);
	monkey1.transformation.updateWorldMatrix();

	monkey2.vertexBuffers = &monkeyBuffers;
	monkey2.transformation.position = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);
	monkey2.transformation.updateWorldMatrix();

	cube.vertexBuffers = &cubeBuffers;
	cube.transformation.scale = XMVectorSet(10.0f, 0.01f, 10.0f, 0.0f);
	cube.transformation.updateWorldMatrix();

	camera.getTransformation().position = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	camera.getTransformation().rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
	camera.updateViewMatrix();
	cameraController.setCamera(&camera);

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
	fw::DX::context->PSSetShaderResources(0, 1, &textureView);
	
	renderObject(monkey1);
	renderObject(monkey2);
	renderObject(cube);	
}

void CSMApp::gui()
{
	ImGui::Text("Hello, world!");
}

bool CSMApp::createMatrixBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(XMMATRIX) * 3;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, nullptr, &matrixBuffer);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create matrix buffer\n";
		return false;
	}
	return true;
}

bool CSMApp::createVertexBuffers(VertexBuffers& vertexBuffers, const std::string& modelFile)
{
	fw::Model model;
	model.loadModel(modelFile);
	std::vector<float> vertexData = fw::getVertexData(model);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(float) * vertexData.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertexData.data();

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &vertexBuffers.vertexBuffer);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create vertex buffer\n";
		return false;
	}

	vertexBuffers.numIndices = model.getNumIndices();
	std::vector<WORD> indices;
	for (const auto& mesh : model.getMeshes()) {
		indices.insert(indices.end(), mesh.indices.begin(), mesh.indices.end());
	}

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * model.getNumIndices();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	data.pSysMem = indices.data();

	hr = fw::DX::device->CreateBuffer(&bd, &data, &vertexBuffers.indexBuffer);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create index buffer\n";
		return false;
	}

	return true;
}

void CSMApp::renderObject(const RenderData& renderData)
{
	VertexBuffers& vb = *renderData.vertexBuffers;
	fw::DX::context->IASetVertexBuffers(0, 1, &vb.vertexBuffer, &vb.stride, &vb.offset);
	fw::DX::context->IASetIndexBuffer(vb.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	fw::DX::context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	MatrixData* matrixData = (MatrixData*)MappedResource.pData;
	matrixData->world = renderData.transformation.getWorldMatrix();
	matrixData->view = camera.getViewMatrix();
	matrixData->projection = camera.getProjectionMatrix();
	fw::DX::context->Unmap(matrixBuffer, 0);

	fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);
	fw::DX::context->DrawIndexed(vb.numIndices, 0, 0);
}
