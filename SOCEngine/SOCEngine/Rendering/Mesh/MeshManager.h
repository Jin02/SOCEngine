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
			Structure::Map<unsigned int, Mesh::Mesh>	_alphaMeshes;
			Structure::Map<unsigned int, Mesh::Mesh>	_nonAlphaMeshes;

		public:
			MeshManager();
			~MeshManager();

		public:
			bool Add(Mesh::Mesh* mesh, MeshType type);
			void Change(Mesh::Mesh* mesh, MeshType type);
			Mesh::Mesh* Find(unsigned int meshAddress, MeshType type);
			Mesh::Mesh* Find(unsigned int meshAddress, MeshType* outType);

		public:
			void Iterate(const std::function<void(Mesh::Mesh* mesh)>& recvFunc, MeshType type) const;
		};
	}
}