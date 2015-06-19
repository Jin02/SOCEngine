#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include "ForwardPlusCamera.h"
#include "BackBufferMaker.h"

#include "GlobalSetting.h"

#include <sys/timeb.h>
#include <time.h>

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

			struct MeshList
			{
				uint updateCounter;
				Structure::Map<unsigned int, const Mesh::Mesh> meshes;

				MeshList(){}
				~MeshList(){meshes.DeleteAll(true);}
			};

		private:
			MeshList	_transparentMeshes;
			MeshList	_opaqueMeshes;

			std::map<std::string, Shader::Shaders>					_physicallyBasedShaders;

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

		public:
			bool Init();

			void UpdateRenderList(const Mesh::Mesh* mesh, MeshType type);
			const Mesh::Mesh* FindMeshFromRenderList(const Mesh::Mesh* mesh, MeshType type);

		public:
			GET_ACCESSOR(TransparentMeshList, const MeshList&, _transparentMeshes);
			GET_ACCESSOR(OpaqueMeshList, const MeshList&, _opaqueMeshes);
		};
	}
}