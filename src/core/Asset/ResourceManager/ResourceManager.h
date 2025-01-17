#pragma once
#include "Utils/SingletonBase.h"
#include "Utils/UUID.hpp"
#include "Asset/AssetManager/AssetManager.h"
#include "Asset/AssetData/AssetData.h"
#include <filesystem>

#include "Render/RenderGraph.hpp"

namespace SPW
{
	class ResourceManager : public SingletonBase<ResourceManager>
	{
	public:
		using textureLoadCallback = std::function<void(int width, int height, int bpp, unsigned char*)>;
		void LoadTextureScope(bool flip, const std::filesystem::path& filePath, const textureLoadCallback& callback);

	public:
		std::unordered_map<std::string, AssetData>	m_AssetDataMap;

		std::unordered_map<std::string, UUID>		m_CameraIDMap;
		UUID activeCameraID;

		std::unordered_map<std::string, Mesh>   m_Skyboxs;
	};
}
