#pragma once

#include "Mesh.h"
#include <vector>

namespace Rendering
{
	namespace Geometry
	{
		class MeshRawPool final
		{
		public:
			MeshRawPool() = default;

			Mesh& Add(Mesh& mesh)	{ _meshes.push_back(mesh);	return _meshes.back();	}
			void Delete(Mesh& mesh) { Delete(mesh.GetObjectID().Literal());				}
			bool Has(Mesh& mesh)	{ Has(mesh.GetObjectID().Literal());				}
			Mesh& Get(uint index)	{ return _meshes[index];							}

			Mesh* Find(Core::ObjectID::LiteralType id);
			void Delete(Core::ObjectID::LiteralType id);
			bool Has(Core::ObjectID::LiteralType id) const;

			void DeleteAll();

			bool Empty() const { return _meshes.empty(); }
			GET_CONST_ACCESSOR(Size, uint, _meshes.size());

		private:
			std::vector<Mesh>	_meshes;
		};
	}
}