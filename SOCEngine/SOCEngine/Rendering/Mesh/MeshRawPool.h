#pragma once

#include "Mesh.h"
#include <vector>
#include <initializer_list>

namespace Rendering
{
	namespace Geometry
	{
		class MeshRawPool final
		{
		public:
			MeshRawPool() = default;
			MeshRawPool(const std::initializer_list<Mesh>& meshes);

			Mesh& Add(Mesh& mesh)	{ _meshes.push_back(mesh);	return _meshes.back();	}
			void Delete(Mesh& mesh) { Delete(mesh.GetObjectID().Literal());				}
			bool Has(Mesh& mesh)	{ Has(mesh.GetObjectID().Literal());				}

			const Mesh* Find(Core::ObjectID::LiteralType id) const;
			Mesh* Find(Core::ObjectID::LiteralType id)
			{
				return const_cast<Mesh*>( static_cast<const MeshRawPool*>(this)->Find(id) );				
			}

			Mesh& Get(uint index)				{ return _meshes[index]; }
			const Mesh& Get(uint index) const	{ return _meshes[index]; }
			
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
