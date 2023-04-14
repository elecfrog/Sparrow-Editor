/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include <glm/glm.hpp>

#include "MaterialData.h"
#include "Asset/Asset.hpp"
#include "Model/Mesh.h"

namespace SPW
{
	struct AssetData
	{
        std::string                                     assetID;
        std::string                                     assetName;
        std::string                                     path;
        std::string                                     meshURI;
        std::vector<Mesh>                               meshes;
        std::vector<MaterialData>                       materials;
        std::unordered_map<std::string, std::string>    textures;
	};
}
