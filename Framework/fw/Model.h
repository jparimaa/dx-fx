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
	unsigned int getNumVertices() const;
	unsigned int getNumIndices() const;

private:
	Meshes meshes;
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;
};

} // fw