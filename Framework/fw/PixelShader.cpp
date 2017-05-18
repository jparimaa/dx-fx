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

bool PixelShader::create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel)
{
	ID3DBlob* blob = nullptr;
	HRESULT hr = compileShaderFromFile(fileName, entryPoint, shaderModel, &blob);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to compile pixel shader\n";
		return false;
	}

	hr = DX::device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);
	blob->Release();

	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create pixel shader\n";
		return false;
	}

	return true;
}

ID3D11PixelShader * PixelShader::get() const
{
	return pixelShader;
}

} // fw