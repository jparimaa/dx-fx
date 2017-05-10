#pragma once

#include "Framework.h"
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>

namespace fw
{

template <typename T>
inline int executeGenericMain(HINSTANCE hInstance, int nCmdShow, LONG windowWidth = 800, LONG windowHeight = 600)
{
	int ret = EXIT_FAILURE;
	Framework framework(windowWidth, windowHeight);
	if (framework.initialize(hInstance, nCmdShow)) {
		T app;
		if (framework.setApplication(&app)) {
			ret = framework.execute();
		}
	}

	if (ret == EXIT_FAILURE) {
		MessageBox(framework.getWindow().getWindowHandle(), L"Fatal error. Check console output.", nullptr, MB_OK);
	}

	return ret;
}

inline HRESULT compileShaderFromFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
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

template <typename T>
inline void release(T* t)
{
	if (t) {
		t->Release();
	}
}

} // fw