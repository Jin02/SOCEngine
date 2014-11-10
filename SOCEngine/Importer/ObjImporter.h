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
		struct CustomSemantic
		{
			std::string		semanticName;
			const void*		data;

			CustomSemantic() : data(nullptr){}
			CustomSemantic(const std::string& _semanticName, const void* _data) : semanticName(_semanticName), data(_data) {}
		};

	private:
		Structure::Map<tinyobj::material_t>	_shapes;

	public:
		ObjImporter();
		~ObjImporter();

	public:
		void LoadMaterial(const std::vector<tinyobj::material_t>& materials, const std::string& fileDir, const std::string& fileName, const std::string& materialFileFolder);
		
	public:
		Core::Object* LoadMesh(const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& fileName, const std::vector<CustomSemantic>& customSemanticData, const Rendering::Shader::VertexShader* vs, bool isDynamicMesh = false);
		Core::Object* LoadMesh(const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& fileName, bool isDynamicMesh = false);

	public:
		Core::Object* Load(const std::string& fileDir, const std::string& fileName, const std::string& materialFileFolder, const Rendering::Shader::VertexShader* vs);
	};
}