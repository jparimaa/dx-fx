#pragma once

#include <d3d11.h>

namespace dfx
{

class PixelShader
{
public:
	PixelShader();
	~PixelShader();

	HRESULT create(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel);
	ID3D11PixelShader* get() const;

private:
	ID3D11PixelShader* pixelShader = nullptr;
};

} // fx