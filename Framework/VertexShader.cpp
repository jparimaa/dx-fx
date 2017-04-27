#include "VertexShader.h"
#include "Common.h"
#include "DX.h"
#include <d3dx11.h>
#include <d3dcompiler.h>

VertexShader::VertexShader()
{
}

VertexShader::~VertexShader()
{
	release(vertexShader);
	release(vertexLayout);
}

HRESULT VertexShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, std::vector<D3D11_INPUT_ELEMENT_DESC> layout)
{
	ID3DBlob* blob = nullptr;
	HRESULT hr = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
	if (FAILED(hr)) {
		MessageBox(NULL, L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = DX::device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr)) {
		blob->Release();
		return hr;
	}

	hr = DX::device->CreateInputLayout(layout.data(), layout.size(), blob->GetBufferPointer(),
									   blob->GetBufferSize(), &vertexLayout);
	blob->Release();
	if (FAILED(hr)) {
		return hr;
	}

	DX::context->IASetInputLayout(vertexLayout);
	return hr;
}
