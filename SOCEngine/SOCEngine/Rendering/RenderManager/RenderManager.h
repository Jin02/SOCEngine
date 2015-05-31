#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include "ForwardPlusCamera.h"
#include "BackBufferMaker.h"

#include "GlobalSetting.h"

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		public:
			enum class MeshType
			{
				Opaque,
				Transparent
			};


		private:
			Structure::MapInMap<unsigned int, std::pair<const Material*, const Mesh::Mesh*>>	_transparentMeshes;
			Structure::MapInMap<unsigned int, std::pair<const Material*, const Mesh::Mesh*>>	_opaqueMeshes;			

			Structure::Map<const std::string, Shader::ShaderGroup> _physicallyBasedShaders;
			Structure::Map<const std::string, Shader::ShaderGroup> _basicShaders;

		public:
			RenderManager();
			~RenderManager();

		public:
			void RenderManager::FindShader(
				const Rendering::Shader::VertexShader**			outVertexShader,
				const Rendering::Shader::PixelShader**			outPixelShader,
				Rendering::Mesh::MeshFilter::BufferElementFlag	bufferFlag,
				Rendering::Material::Type materialType,
				RenderType renderType = DEFAULT_USER_RENDER_TYPE,
				const std::string& frontShaderTypeName = "");

			void Iterate(const std::function<void(const Material* material, const Mesh::Mesh* mesh)>& recvFunc, MeshType type) const;

		public:
			bool Init();

			bool Add(const Material* material, const Mesh::Mesh* mesh, MeshType type);
			void Change(const Material* material, const Mesh::Mesh* mesh, MeshType type);
			std::pair<const Material*, const Mesh::Mesh*>* Find(const Material* material, const Mesh::Mesh* mesh, MeshType type);

		public:
			void ScreenMerge();
		};
	}
}