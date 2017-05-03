#include "PixelShader.h"
#include "Common.h"
#include "DX.h"
#include <d3dx11.h>
#include <d3dcompiler.h>

namespace fw
{

PixelShader::PixelShader()
{
}

PixelShader::~PixelShader()
{
	release(pixelShader);
}

HRESULT PixelShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel)
{
	ID3DBlob* blob = nullptr;
	HRESULT hr = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = DX::device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);
	blob->Release();
	return hr;
}

ID3D11PixelShader * PixelShader::get() const
{
	return pixelShader;
}

} // fx