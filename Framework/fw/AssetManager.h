#pragma once

#include "Common.h"
#include <d3d11.h>
#include <unordered_map>
#include <string>

namespace fw
{

class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	
	ID3D11ShaderResourceView* getTextureView(const std::string& filename);
	VertexBuffer* getVertexBuffer(const std::string& filename);
	bool getLinearSampler(ID3D11SamplerState** sampler);

private:
	struct TextureData
	{
		ID3D11Resource* texture = nullptr;
		ID3D11ShaderResourceView* textureView = nullptr;
	};

	std::unordered_map<std::string, TextureData> textures;
	std::unordered_map<std::string, VertexBuffer> modelBuffers;
	ID3D11SamplerState* samplerLinear = nullptr;
};

} // fw