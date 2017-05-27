#include "ExampleApp.h"
#include <fw/Model.h>
#include <fw/Common.h>
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

ExampleApp::ExampleApp()
{
}

ExampleApp::~ExampleApp()
{
	fw::release(vertexBuffer);
	fw::release(indexBuffer);
	fw::release(matrixBuffer);
	fw::release(texture);
	fw::release(textureView);
	fw::release(samplerLinear);
}

bool ExampleApp::initialize()
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

	if (!createBuffer()) {
		return false;
	}

	camera.getTransformation().position = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	camera.getTransformation().rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
	camera.updateViewMatrix();
	cameraController.setCamera(&camera);

	HRESULT hr = DirectX::CreateWICTextureFromFile(fw::DX::device, L"../Assets/green_square.png", &texture, &textureView);
	if (FAILED(hr)) {
		std::cerr << "ERRROR: Failed to create WIC texture from file\n";
		return false;
	}
	if (!fw::getLinearSampler(&samplerLinear)) {
		return false;
	}

	std::cout << "ExampleApp initialization completed\n";

	return true;
}

void ExampleApp::update()
{
	if (fw::API::isKeyReleased(DirectX::Keyboard::Escape)) {
		fw::API::quit();
	}

	cameraController.update();
	camera.updateViewMatrix();

	trans.rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), XM_2PI * fw::API::getTimeDelta() * 0.1f);
	trans.updateWorldMatrix();
	XMMATRIX m[] = {
		trans.getWorldMatrix(),
		camera.getViewMatrix(),
		camera.getProjectionMatrix()
	};

	fw::DX::context->UpdateSubresource(matrixBuffer, 0, NULL, &m, 0, 0);
}

void ExampleApp::render()
{
	fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
	fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
	fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);
	fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);

	fw::DX::context->PSSetShaderResources(0, 1, &textureView);
	fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);

	fw::DX::context->DrawIndexed(numIndices, 0, 0);	
}

void ExampleApp::gui()
{
	ImGui::Text("Hello, world!");
}

bool ExampleApp::createBuffer()
{
	fw::Model model;
	model.loadModel("../Assets/monkey.3ds");
	std::vector<float> vertexData;
	for (const auto& mesh : model.getMeshes()) {
		for (unsigned int i = 0; i < mesh.vertices.size(); ++i) {
			vertexData.push_back(mesh.vertices[i].x);
			vertexData.push_back(mesh.vertices[i].y);
			vertexData.push_back(mesh.vertices[i].z);
			vertexData.push_back(mesh.normals[i].x);
			vertexData.push_back(mesh.normals[i].y);
			vertexData.push_back(mesh.normals[i].z);
			vertexData.push_back(mesh.uvs[i].x);
			vertexData.push_back(mesh.uvs[i].y);
		}
	}

	// Vertex data
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(float) * vertexData.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertexData.data();

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &vertexBuffer);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create vertex buffer\n";
		return false;
	}

	UINT stride = 8 * sizeof(float);
	UINT offset = 0;
	fw::DX::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	
	// Index data
	numIndices = model.getNumIndices();
	std::vector<WORD> indices;
	for (const auto& mesh : model.getMeshes()) {
		indices.insert(indices.end(), mesh.indices.begin(), mesh.indices.end());
	}

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * model.getNumIndices();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	data.pSysMem = indices.data();

	hr = fw::DX::device->CreateBuffer(&bd, &data, &indexBuffer);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create index buffer\n";
		return false;
	}

	fw::DX::context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);	
	fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Transformation matrix data
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMMATRIX) * 3;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = fw::DX::device->CreateBuffer(&bd, NULL, &matrixBuffer);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create matrix buffer\n";
		return false;
	}

	return true;
}
