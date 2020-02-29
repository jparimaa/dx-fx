#pragma once

#include <assimp/material.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace fw
{
class Model
{
public:
    struct Mesh
    {
        std::vector<DirectX::XMFLOAT3> vertices;
        std::vector<DirectX::XMFLOAT3> normals;
        std::vector<DirectX::XMFLOAT3> tangents;
        std::vector<DirectX::XMFLOAT2> uvs;
        std::vector<WORD> indices;

        using Materials = std::unordered_map<aiTextureType, std::vector<std::string>>;
        Materials materials;
    };

    using Meshes = std::vector<Mesh>;

    explicit Model();
    ~Model();

    bool loadModel(const std::string& file);
    const Meshes& getMeshes() const;
    size_t getNumVertices() const;
    size_t getNumIndices() const;

private:
    Meshes meshes;
    size_t numVertices = 0;
    size_t numIndices = 0;
};

} // namespace fw