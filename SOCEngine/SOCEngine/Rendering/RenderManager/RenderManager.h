#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include "ForwardPlusCamera.h"

#include "VertexShader.h"
#include "PixelShader.h"

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
				hasAlpha,
				nonAlpha
			};


		private:
			Structure::MapInMap<unsigned int, std::pair<const Material*, const Mesh::Mesh*>>	_alphaMeshes;
			Structure::MapInMap<unsigned int, std::pair<const Material*, const Mesh::Mesh*>>	_nonAlphaMeshes;			

			Structure::Map<const std::string, Shader::RenderShaders> _physicallyBasedShaders;
			Structure::Map<const std::string, Shader::RenderShaders> _basicShaders;

		public:
			RenderManager();
			~RenderManager();

		private:
			void Iterate(const std::function<void(const Material* material, const Mesh::Mesh* mesh)>& recvFunc, MeshType type) const;

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

			bool Add(const Material* material, const Mesh::Mesh* mesh, MeshType type);
			void Change(const Material* material, const Mesh::Mesh* mesh, MeshType type);
			std::pair<const Material*, const Mesh::Mesh*>* Find(const Material* material, const Mesh::Mesh* mesh, MeshType type);

		private:
			void ForwardPlusRender(ID3D11DeviceContext* context, const Camera::ForwardPlusCamera* camera);

		public:
			void Render(const Camera::Camera* camera);
		};
	}
}