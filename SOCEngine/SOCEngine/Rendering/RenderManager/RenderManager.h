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
			std::vector<const Mesh::Mesh*>		_transparentMeshes;
			std::vector<const Mesh::Mesh*>		_opaqueMeshes;

			std::map<std::string, Shader::Shaders>	_physicallyBasedShaders;

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

			void Iterate(const std::function<void(const Mesh::Mesh* mesh)>& recvFunc, MeshType type, Camera::Camera* camera);

		public:
			bool Init();
		};
	}
}