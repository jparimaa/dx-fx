#include "Common.h"

namespace fw
{

HRESULT compileShaderFromFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#if defined(DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errorBlob;
	hr = D3DX11CompileFromFile(fileName, nullptr, nullptr, entryPoint, shaderModel,
							   shaderFlags, 0, nullptr, blobOut, &errorBlob, nullptr);
	if (FAILED(hr)) {
		if (errorBlob != nullptr) {
			std::cerr << "ERROR: " << static_cast<char*>(errorBlob->GetBufferPointer()) << "\n";;
		}
	}
	if (errorBlob) {
		errorBlob->Release();
	}

	return hr;
}

bool getLinearSampler(ID3D11SamplerState** sampler)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = fw::DX::device->CreateSamplerState(&sampDesc, sampler);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create sampler state\n";
		return false;
	}
	return true;
}

} // fw
