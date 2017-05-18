#include "VertexShader.h"
#include "Common.h"
#include "DX.h"
#include <d3dx11.h>
#include <d3dcompiler.h>

namespace fw
{

VertexShader::VertexShader()
{
}

VertexShader::~VertexShader()
{
	release(vertexShader);
	release(vertexLayout);
}

bool VertexShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, std::vector<D3D11_INPUT_ELEMENT_DESC> layout)
{
	ID3DBlob* blob = nullptr;
	HRESULT hr = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
	if (FAILED(hr)) {
		std::wcerr << "ERROR: Failed to compile vertex shader\n";
		return false;
	}

	hr = DX::device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(hr)) {
		std::wcerr << "ERROR: Failed to create vertex shader\n";
		blob->Release();
		return false;
	}

	hr = DX::device->CreateInputLayout(layout.data(), layout.size(), blob->GetBufferPointer(),
									   blob->GetBufferSize(), &vertexLayout);
	blob->Release();
	if (FAILED(hr)) {
		std::wcerr << "ERROR: Failed to create input layout for vertex shader\n";
		return false;
	}

	DX::context->IASetInputLayout(vertexLayout);
	return true;
}

ID3D11VertexShader * VertexShader::get() const
{
	return vertexShader;
}

} // fw