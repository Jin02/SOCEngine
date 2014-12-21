#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

namespace Rendering
{
	namespace Manager
	{
		class MeshManager
		{
		public:
			enum MeshType
			{
				hasAlpha,
				nonAlpha
			};

		private:
			Structure::MapInMap<unsigned int, std::pair<Material*, Mesh::Mesh*>>	_alphaMeshes;
			Structure::MapInMap<unsigned int, std::pair<Material*, Mesh::Mesh*>>	_nonAlphaMeshes;
			 
		public:
			MeshManager();
			~MeshManager();

		public:
			bool Add(Material* material, Mesh::Mesh* mesh, MeshType type);
			void Change(const Material* material, const Mesh::Mesh* mesh, MeshType type);
			std::pair<Material*, Mesh::Mesh*>* Find(Material* material, Mesh::Mesh* mesh, MeshType type);

		public:
			void Iterate(const std::function<void(Material* material, Mesh::Mesh* mesh)>& recvFunc, MeshType type) const;
		};
	}
}