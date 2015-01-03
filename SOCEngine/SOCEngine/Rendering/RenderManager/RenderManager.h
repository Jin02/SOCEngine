#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		public:
			enum MeshType
			{
				hasAlpha,
				nonAlpha
			};

		private:
			Structure::MapInMap<unsigned int, std::pair<BasicMaterial*, Mesh::Mesh*>>	_alphaMeshes;
			Structure::MapInMap<unsigned int, std::pair<BasicMaterial*, Mesh::Mesh*>>	_nonAlphaMeshes;
			 
		public:
			RenderManager();
			~RenderManager();

		public:
			bool Add(BasicMaterial* material, Mesh::Mesh* mesh, MeshType type);
			void Change(const BasicMaterial* material, const Mesh::Mesh* mesh, MeshType type);
			std::pair<BasicMaterial*, Mesh::Mesh*>* Find(BasicMaterial* material, Mesh::Mesh* mesh, MeshType type);

		public:
			void Iterate(const std::function<void(BasicMaterial* material, Mesh::Mesh* mesh)>& recvFunc, MeshType type) const;
		};
	}
}