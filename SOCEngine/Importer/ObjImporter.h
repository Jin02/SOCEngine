#pragma once

#include <string>

#include "Vector2.h"
#include "Vector3.h"

#include "MaterialManager.h"
#include "TextureManager.h"

#include "tiny_obj_loader.h"
#include "Object.h"

namespace Importer
{
	class ObjImporter
	{
	public:
		ObjImporter();
		~ObjImporter();

	public:
		Core::Object* Load(const std::string& fileFolderPath, const std::string& fileNameWithExtension, bool useNormalMap);
	};
}