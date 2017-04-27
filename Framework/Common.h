#pragma once

#include <d3dx11.h>
#include <d3dcompiler.h>

inline HRESULT compileShaderFromFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errorBlob;
	hr = D3DX11CompileFromFile(fileName, NULL, NULL, entryPoint, shaderModel,
							   shaderFlags, 0, NULL, blobOut, &errorBlob, NULL);
	if (FAILED(hr)) {
		if (errorBlob != NULL) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
	}
	if (errorBlob) {
		errorBlob->Release();
	}

	return hr;
}

template <typename T>
inline void release(T* t)
{
	if (t) {
		t->Release();
	}
}