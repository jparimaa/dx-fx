#pragma once

#include "Framework.h"
#include "DX.h"
#include "Model.h"
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <vector>

namespace fw
{

void printHresult(HRESULT* hr, std::ostream& os);
void printError(const std::string& msg, HRESULT* hr = nullptr);
void printWarning(const std::string& msg, HRESULT* hr = nullptr);

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
		MessageBox(framework.getWindow()->getHandle(), L"Fatal error. Check console output.", nullptr, MB_OK);
	}

	return ret;
}

template <typename T>
inline void release(T* t)
{
	if (t) {
		t->Release();
		t = nullptr;
	}	
}

bool compileShaderFromFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut);

bool getLinearSampler(ID3D11SamplerState** sampler);

std::vector<float> getVertexData(const fw::Model& model);

} // fw