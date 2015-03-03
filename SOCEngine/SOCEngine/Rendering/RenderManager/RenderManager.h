#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>
#include "Camera.h"

#include "VertexShader.h"
#include "PixelShader.h"

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		private:
			enum class MeshType
			{
				hasAlpha,
				nonAlpha
			};

			struct Shaders
			{
				Shader::VertexShader*	vs;
				Shader::PixelShader*	ps;

				Shaders() : vs(nullptr), ps(nullptr) {}
				Shaders(Shader::VertexShader* _vs, Shader::PixelShader *_ps) : vs(_vs), ps(_ps) {}
				~Shaders() {}
			};


		private:
			Structure::MapInMap<unsigned int, std::pair<Material*, Mesh::Mesh*>>	_alphaMeshes;
			Structure::MapInMap<unsigned int, std::pair<Material*, Mesh::Mesh*>>	_nonAlphaMeshes;			

			Structure::Map<const std::string, Shaders> _physicallyBasedShaders;
			Structure::Map<const std::string, Shaders> _basicShaders;

		public:
			RenderManager();
			~RenderManager();

		private:
			void Iterate(const std::function<void(Material* material, Mesh::Mesh* mesh)>& recvFunc, MeshType type) const;

		public:
			bool Init();

			bool Add(Material* material, Mesh::Mesh* mesh, MeshType type);
			void Change(const Material* material, const Mesh::Mesh* mesh, MeshType type);
			std::pair<Material*, Mesh::Mesh*>* Find(Material* material, Mesh::Mesh* mesh, MeshType type);

			void Render(const Camera::Camera* camera);
		};
	}
}