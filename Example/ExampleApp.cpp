#include "ExampleApp.h"
#include <Common.h>
#include <DX.h>
#include <xnamath.h>
#include <windows.h>
#include <vector>

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
}

bool ExampleApp::initialize()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

	return true;
}

void ExampleApp::update()
{
}

void ExampleApp::render()
{
	fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);

	fw::DX::context->VSSetShader(vertexShader.get(), nullptr, 0);
	fw::DX::context->PSSetShader(pixelShader.get(), nullptr, 0);
	fw::DX::context->Draw(3, 0);

	fw::DX::swapChain->Present(0, 0);
}

void ExampleApp::gui()
{
}

bool ExampleApp::createBuffer()
{
	XMFLOAT3 vertices[] = {
		XMFLOAT3(0.0f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f),
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT3) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertices;

	HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &vertexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	fw::DX::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return true;
}
