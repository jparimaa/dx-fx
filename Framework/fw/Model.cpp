#include "Model.h"
#include "Common.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <utility>

namespace fw
{
Model::Model()
{
}

Model::~Model()
{
}

bool Model::loadModel(const std::string& file)
{
    numVertices = 0;
    numIndices = 0;

    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
    const aiScene* aScene = importer.ReadFile(file, flags);

    if (aScene)
    {
        if (aScene->mNumMeshes == 0)
        {
            printWarning("No mesh found in the file: " + file);
            return false;
        }

        for (unsigned int meshIndex = 0; meshIndex < aScene->mNumMeshes; ++meshIndex)
        {
            const aiMesh* aMesh = aScene->mMeshes[meshIndex];
            Mesh mesh;
            for (unsigned int vertexIndex = 0; vertexIndex < aMesh->mNumVertices; ++vertexIndex)
            {
                mesh.vertices.emplace_back(aMesh->mVertices[vertexIndex].x,
                                           aMesh->mVertices[vertexIndex].y,
                                           aMesh->mVertices[vertexIndex].z);

                mesh.normals.emplace_back(aMesh->mNormals[vertexIndex].x,
                                          aMesh->mNormals[vertexIndex].y,
                                          aMesh->mNormals[vertexIndex].z);

                if (aMesh->HasTangentsAndBitangents())
                {
                    mesh.tangents.emplace_back(aMesh->mTangents[vertexIndex].x,
                                               aMesh->mTangents[vertexIndex].y,
                                               aMesh->mTangents[vertexIndex].z);
                }

                if (aMesh->HasTextureCoords(0))
                {
                    mesh.uvs.emplace_back(aMesh->mTextureCoords[0][vertexIndex].x,
                                          aMesh->mTextureCoords[0][vertexIndex].y);
                }
            }

            for (unsigned int faceIndex = 0; faceIndex < aMesh->mNumFaces; ++faceIndex)
            {
                if (aMesh->mFaces[faceIndex].mNumIndices != 3)
                {
                    printWarning("Unable to parse model indices for file: " + file);
                    return false;
                }
                mesh.indices.push_back(static_cast<WORD>(aMesh->mFaces[faceIndex].mIndices[0]));
                mesh.indices.push_back(static_cast<WORD>(aMesh->mFaces[faceIndex].mIndices[1]));
                mesh.indices.push_back(static_cast<WORD>(aMesh->mFaces[faceIndex].mIndices[2]));
            }

            aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];
            if (aMaterial)
            {
                for (int typeIndex = 0; typeIndex < aiTextureType_UNKNOWN; ++typeIndex)
                {
                    aiTextureType type = static_cast<aiTextureType>(typeIndex);
                    unsigned int numTextures = aMaterial->GetTextureCount(type);
                    for (unsigned int texIndex = 0; texIndex < numTextures; ++texIndex)
                    {
                        aiString path;
                        aMaterial->GetTexture(type, 0, &path);
                        mesh.materials[type].emplace_back(std::string(path.C_Str()));
                    }
                }
            }

            if (mesh.vertices.empty())
            {
                printWarning("Invalid mesh, 0 vertices");
                return false;
            }
            numVertices += mesh.vertices.size();
            numIndices += mesh.indices.size();
            meshes.push_back(std::move(mesh));
        }
    }
    else
    {
        printWarning("Failed to read model: " + file + "\n" + "Importer error message: " + importer.GetErrorString() + "\n");
        return false;
    }

    if (meshes.empty())
    {
        printWarning("Empty model");
        return false;
    }
    return true;
}

const Model::Meshes& Model::getMeshes() const
{
    return meshes;
}

size_t Model::getNumVertices() const
{
    return numVertices;
}

size_t Model::getNumIndices() const
{
    return numIndices;
}

} // namespace fw