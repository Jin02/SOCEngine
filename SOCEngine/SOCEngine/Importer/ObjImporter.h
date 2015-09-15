#pragma once

#include <string>
#include <functional>

#include "Vector2.h"
#include "Vector3.h"

#include "MaterialManager.h"
#include "TextureManager.h"

#include "tiny_obj_loader.h"
#include "Object.h"

namespace Importer
{
	namespace Obj
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
			const std::function<Core::Object*(const std::string& key)>					_findOriginObjFunc;
			const std::function<void(const std::string& key, Core::Object* obj)>	_addOriginObject;

		public:
			ObjImporter(
				const std::function<Core::Object*(const std::string& key)>& findOriginObjFunc,
				const std::function<void(const std::string& key, Core::Object* obj)>& addOriginObjectFunc);
			~ObjImporter();

		private:		
			Core::Object* CloneOriginObject(const std::string& fileName, const std::string& tinyShapeName);
			void CheckCorrectShape(const tinyobj::shape_t& tinyShape);

		private:
			Core::Object* LoadMesh(
				const tinyobj::shape_t& tinyShape,
				const tinyobj::material_t& tinyMtl,
				const std::string& fileName, 
				Rendering::Material::Type materialType,
				bool isDynamicMesh = false);

		public:
			Rendering::Material* LoadMaterial(const tinyobj::material_t& tinyMaterial, const std::string& fileName, const std::string& materialFileFolder, Rendering::Material::Type materialType);
			void LoadMaterials(const std::vector<tinyobj::material_t>& tinyMaterials, const std::string& fileName, const std::string& materialFileFolder, Rendering::Material::Type materialType);

			Core::Object* LoadMesh(const tinyobj::shape_t& tinyShape, 
									const tinyobj::material_t& tinyMtl, 
									const std::string& fileName,
									const std::vector<Rendering::Shader::VertexShader::SemanticInfo>& semanticInfos,
									Rendering::Material::Type materialType,
									bool isDynamicMesh = false);

			Core::Object* Load(
				const std::string& fileDir,
				const std::string& fileName, 
				const std::string& materialFileFolder, 
				Rendering::Material::Type materialType,
				bool isDynamicMesh = false);

			bool Load(std::vector<tinyobj::shape_t>& outShapes, std::vector<tinyobj::material_t>& outMaterials, const std::string& fileDir, const std::string& materialFileFolder);
		};
	}
}