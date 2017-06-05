#pragma once

#include <d3d11.h>
#include <unordered_map>
#include <string>

namespace fw
{

class AssetManager
{
public:
	struct VertexBuffer
	{
		ID3D11Buffer* vertexBuffer = nullptr;
		UINT stride = 8 * sizeof(float);
		UINT offset = 0;
		ID3D11Buffer* indexBuffer = nullptr;
		unsigned int numIndices = 0;
	};

	AssetManager();
	~AssetManager();
	
	ID3D11ShaderResourceView* getTextureView(const std::string& filename);
	VertexBuffer* getVertexBuffer(const std::string& filename);

private:
	struct TextureData
	{
		ID3D11Resource* texture = nullptr;
		ID3D11ShaderResourceView* textureView = nullptr;
	};

	std::unordered_map<std::string, TextureData> textures;
	std::unordered_map<std::string, VertexBuffer> modelBuffers;
};

} // fw