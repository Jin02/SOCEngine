#pragma once

#include <string>

#include "Vector2.h"
#include "Vector3.h"

#include "MaterialManager.h"
#include "TextureManager.h"

namespace Importer
{
	class ObjImporter
	{
	private:
		struct ObjTriangle
		{
			int vertex[3];
			int normal[3];
			int uvs[3];
		};

	public:
		ObjImporter();
		~ObjImporter();

	private:
		bool LoadMaterials(const std::string& fileName, const std::string& fileFolderPath, Rendering::Material::MaterialManager& materialMgr, Rendering::Texture::TextureManager& textureMgr);

	public:
		bool Load(const std::string& fileName, const std::string& folderPath, Rendering::Material::MaterialManager& materialMgr, Rendering::Texture::TextureManager& textureMgr);
	};
}