#pragma once

#include "VectorIndexer.hpp"
#include "Mesh.h"
#include "ObjectId.hpp"
#include <tuple>
#include <assert.h>

namespace Rendering
{
	namespace Manager
	{
		class MeshManager final
		{
		public:
			MeshManager() = default;

			template <typename Trait>
			class MeshPool : public Core::VectorHashMap<Core::ObjectId::LiteralType, Geometry::Mesh>
			{
			public:
				MeshPool() = default;
				DISALLOW_ASSIGN(MeshPool);
			};
			DISALLOW_ASSIGN_COPY(MeshManager);

			struct MeshTraits {};
			struct Opaque : public MeshTraits {};
			struct AlphaBlend : public MeshTraits {};
			struct Transparency : public MeshTraits {};

			template <typename Trait>
			MeshPool<Trait>& GetPool()
			{
				return std::get<MeshPool<Trait>>(_tuple);
			}
			template <typename Trait>
			const MeshPool<Trait>& GetPool() const
			{
				return std::get<MeshPool<Trait>>(_tuple);
			}

			template <typename FromTrait, typename ToTrait>
			bool Move(Core::ObjectId id)
			{
				auto& fromIndexer	= GetPool<FromTrait>().GetIndexer();
				auto& toIndexer		= GetPool<ToTrait>().GetIndexer();

				assert(fromIndexer.Has(id) == false);
				assert(toIndexer.Has(id) == false);

				uint idx = fromIndexer.Find(id);
				GetPool<ToTrait>().Add(id, GetPool<FromTrait>().Get(idx));
				GetPool<FromTrait>().Delete(id);
			}

		private:
			std::tuple<MeshPool<Opaque>, MeshPool<AlphaBlend>, MeshPool<Transparency>> _tuple;
		};
	}
}