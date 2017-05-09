#include "ExampleApp.h"
#include <Common.h>
#include <DX.h>
#include <windows.h>
#include <vector>
#include <iostream>

namespace
{

float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

} // anonymous

ExampleApp::ExampleApp() :
	api(getAPI())
{
}

ExampleApp::~ExampleApp()
{
	fw::release(vertexBuffer);
	fw::release(indexBuffer);
	fw::release(matrixBuffer);
}

bool ExampleApp::initialize()
{
	api = getAPI();
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(vertexShader.create(L"example.fx", "VS", "vs_4_0", layout))) {
		return false;
	}

	if (FAILED(pixelShader.create(L"example.fx", "PS", "ps_4_0"))) {
		return false;
	}

	if (!createBuffer()) {
		return false;
	}

	camera.getTransformation().position = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	camera.getTransformation().rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
	camera.updateViewMatrix();

	std::cout << "ExampleApp initialization completed\n";

	return true;
}

void ExampleApp::update()
{	
	trans.rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), XM_2PI * api->getTimeDelta());
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

	fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
	fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);
	fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);

	fw::DX::context->DrawIndexed(36, 0, 0);
	fw::DX::swapChain->Present(0, 0);
}

void ExampleApp::gui()
{
}

bool ExampleApp::createBuffer()
{
	float vertices[] = {
		-1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 1.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 0.0f, 1.0f
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(float) * 7 * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertices;

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &vertexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	UINT stride = sizeof(float) * 7;
	UINT offset = 0;
	fw::DX::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	
	WORD indices[] = {
		3,1,0, 2,1,3,
		0,5,4, 1,5,0,
		3,4,7, 0,4,3,
		1,6,5, 2,6,1,
		2,7,6, 3,7,2,
		6,4,5, 7,4,6
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	data.pSysMem = indices;

	hr = fw::DX::device->CreateBuffer(&bd, &data, &indexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	fw::DX::context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMMATRIX) * 3;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = fw::DX::device->CreateBuffer(&bd, NULL, &matrixBuffer);
	if (FAILED(hr)) {
		return false;
	}
	
	return true;
}
