#pragma once

#include <d3d11.h>
#include <vector>

namespace dfx
{

class VertexShader
{
public:
	VertexShader();
	~VertexShader();

	HRESULT create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, std::vector<D3D11_INPUT_ELEMENT_DESC> layout);

private:
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;
};

} // fx