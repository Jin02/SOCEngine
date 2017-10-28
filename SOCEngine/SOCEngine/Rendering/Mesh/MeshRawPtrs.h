#pragma once

#include "Mesh.h"
#include <vector>
#include <initializer_list>

namespace Rendering
{
	namespace Geometry
	{
		class MeshRawPtrs final
		{
		public:
			using PtrType		= const Mesh*;
			using MappedType	= PtrType;
			
			MeshRawPtrs() = default;
			MeshRawPtrs(const std::initializer_list<PtrType>& meshes);

			PtrType		Add(PtrType mesh)			{ _meshes.push_back(mesh);	return _meshes.back();	}
			void		Delete(PtrType mesh)		{ Delete(mesh->GetObjectID().Literal());			}
			bool		Has(PtrType mesh)	const	{ Has(mesh->GetObjectID().Literal());				}

			const PtrType Get(uint index) const		{ return _meshes[index]; }
			const PtrType Find(Core::ObjectID::LiteralType id) const;

			void Delete(Core::ObjectID::LiteralType id);
			bool Has(Core::ObjectID::LiteralType id) const;

			void DeleteAll();

			bool Empty() const { return _meshes.empty(); }
			GET_CONST_ACCESSOR(Size, uint, _meshes.size());

		private:
			std::vector<PtrType>	_meshes;
		};
	}
}
