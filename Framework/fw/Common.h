#pragma once

#include "Framework.h"
#include "DX.h"
#include "Model.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <vector>

namespace fw
{
struct VertexBuffer
{
    ID3D11Buffer* vertexBuffer = nullptr;
    UINT stride = 8 * sizeof(float);
    UINT offset = 0;
    ID3D11Buffer* indexBuffer = nullptr;
    size_t numIndices = 0;
    bool releaseOnDestruction = false;

    ~VertexBuffer();
};

void printHresult(HRESULT* hr, std::wostream& os);
void printError(const std::string& msg, HRESULT* hr = nullptr);
void printWarning(const std::string& msg, HRESULT* hr = nullptr);

template<typename T>
inline int executeGenericMain(HINSTANCE hInstance, int nCmdShow, LONG windowWidth = 800, LONG windowHeight = 600)
{
    int ret = EXIT_FAILURE;
    Framework framework(windowWidth, windowHeight);
    if (framework.initialize(hInstance, nCmdShow))
    {
        T app;
        if (framework.setApplication(&app))
        {
            ret = framework.execute();
        }
    }

    if (ret == EXIT_FAILURE)
    {
        MessageBox(framework.getWindow()->getHandle(), "Fatal error. Check console output.", nullptr, MB_OK);
    }

    return ret;
}

template<typename T>
inline void release(T* t)
{
    if (t)
    {
        t->Release();
        t = nullptr;
    }
}

inline wchar_t* toWchar(const char* c)
{
    const size_t size = strlen(c) + 1;
    wchar_t* wc = new wchar_t[size];
    mbstowcs(wc, c, size);
    return wc;
}

bool compileShaderFromFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut);

std::vector<float> getVertexData(const fw::Model& model);

void displayVector(const std::string& text, const DirectX::XMVECTOR& vector);

} // namespace fw