#include "Common.h"
#include <comdef.h>

namespace fw
{

void printHresult(HRESULT* hr, std::wostream& os)
{
	if (hr) {
		os << "HRESULT: " << _com_error(*hr).ErrorMessage() << "\n";
	}
}

void printError(const std::string& msg, HRESULT* hr)
{
	std::cerr << "ERROR: " << msg << "\n";
	printHresult(hr, std::wcerr);
}

void printWarning(const std::string& msg, HRESULT* hr)
{
	std::cerr << "WARNING: " << msg << "\n";
	printHresult(hr, std::wcerr);
}

bool compileShaderFromFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
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
			printError(static_cast<char*>(errorBlob->GetBufferPointer()), &hr);
		}
	}
	if (errorBlob) {
		errorBlob->Release();
	}

	return !FAILED(hr);
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
		printError("Failed to create sampler state", &hr);
		return false;
	}
	return true;
}

std::vector<float> getVertexData(const fw::Model& model)
{
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
	return vertexData;
}

} // fw
