#pragma once

#include <fw/Common.h>
#include <fw/WcharHelper.h>

#include <d3d11.h>
#include <fileapi.h>

#include <string>

template<typename U>
U* createShader(ID3D11Device* device, ID3DBlob* blob)
{
    printError("Invalid shader type");
    return nullptr;
}

template<>
inline ID3D11VertexShader* createShader(ID3D11Device* device, ID3DBlob* blob)
{
    ID3D11VertexShader* shader = nullptr;
    const HRESULT result = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
    if (FAILED(result))
    {
        return nullptr;
    }
    return shader;
}

template<>
inline ID3D11PixelShader* createShader(ID3D11Device* device, ID3DBlob* blob)
{
    ID3D11PixelShader* shader = nullptr;
    const HRESULT result = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
    if (FAILED(result))
    {
        return nullptr;
    }
    return shader;
}

template<>
inline ID3D11ComputeShader* createShader(ID3D11Device* device, ID3DBlob* blob)
{
    ID3D11ComputeShader* shader = nullptr;
    const HRESULT result = device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
    if (FAILED(result))
    {
        return nullptr;
    }
    return shader;
}

template<typename T>
class ShaderReloader
{
public:
    void init(ID3D11Device* device, const std::string& filepath, const std::string& entry, const std::string& target);
    ~ShaderReloader();
    void recompileIfChanged();
    T* getShader() const;

private:
    ID3D11Device* m_device = nullptr;
    std::string m_filepath;
    std::string m_entry;
    std::string m_target;
    FILETIME m_lastFiletime;
    T* m_shader = nullptr;
};

template<typename T>
void ShaderReloader<T>::init(ID3D11Device* device, const std::string& filepath, const std::string& entry, const std::string& target)
{
    m_device = device;
    m_filepath = filepath;
    m_entry = entry;
    m_target = target;
}

template<typename T>
ShaderReloader<T>::~ShaderReloader()
{
    fw::release(m_shader);
}

template<typename T>
void ShaderReloader<T>::recompileIfChanged()
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributeData{};
    GetFileAttributesExA(m_filepath.c_str(), GetFileExInfoStandard, &fileAttributeData);
    const FILETIME currentTime = fileAttributeData.ftLastWriteTime;
    if (m_lastFiletime.dwLowDateTime == currentTime.dwLowDateTime && m_lastFiletime.dwHighDateTime == currentTime.dwHighDateTime)
    {
        return;
    }

    m_lastFiletime = currentTime;

    ID3DBlob* blob = nullptr;
    fw::ToWchar toWchar(m_filepath);
    bool compiled = fw::compileShaderFromFile(toWchar.getWchar(), m_entry.c_str(), m_target.c_str(), &blob);
    if (!compiled)
    {
        fw::printError("Failed to compile shader in shader reloader");
        return;
    }

    T* shader = createShader<T>(m_device, blob);

    if (shader == nullptr)
    {
        fw::printError("Failed create shader");
        return;
    }

    std::cout << "Shader recompiled " << m_filepath << "\n";
    m_shader = shader;
}

template<typename T>
T* ShaderReloader<T>::getShader() const
{
    return m_shader;
}
