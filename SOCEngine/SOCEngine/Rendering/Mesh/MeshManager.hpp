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
		struct MeshTraits {};
		struct OpaqueTrait : public MeshTraits {};
		struct AlphaBlendTrait : public MeshTraits {};
		struct TransparencyTrait : public MeshTraits {};

		template <typename Trait>
		class MeshPool : public Core::VectorHashMap<Core::ObjectId::LiteralType, Geometry::Mesh>
		{
		public:
			MeshPool() = default;
			DISALLOW_ASSIGN(MeshPool);
		};

		class MeshManager final
		{
		public:
			MeshManager() = default;
			DISALLOW_ASSIGN_COPY(MeshManager);

		public:
			template<typename MeshTraits> Geometry::Mesh& Acquire(Core::ObjectId objId)
			{
				auto mesh = Geometry::Mesh(objId);
				return GetPool<MeshTraits>().Add(objId.Literal(), mesh);
			}
			template<typename MeshTraits> void Delete(Core::ObjectId objId)
			{
				GetPool<MeshTraits>().Delete(objId.Literal());
			}
			template<typename MeshTraits> bool Has(Core::ObjectId objId) const
			{
				return	GetPool<MeshTraits>().GetIndexer().Has(objId.Literal());
			}
			template<typename MeshTraits> auto Find(Core::ObjectId id)
			{
				return GetPool<MeshTraits>().Find(id.Literal());
			}

			Geometry::Mesh& Acquire(Core::ObjectId objId)
			{
				auto mesh = Geometry::Mesh(objId);
				return GetPool<OpaqueTrait>().Add(objId.Literal(), mesh);
			}
			void Delete(Core::ObjectId objId)
			{
				GetPool<OpaqueTrait>().Delete(objId.Literal());
				GetPool<AlphaBlendTrait>().Delete(objId.Literal());
				GetPool<TransparencyTrait>().Delete(objId.Literal());
			}
			bool Has(Core::ObjectId objId) const
			{
				return	GetPool<OpaqueTrait>().GetIndexer().Has(objId.Literal())		|
						GetPool<AlphaBlendTrait>().GetIndexer().Has(objId.Literal())	|
						GetPool<TransparencyTrait>().GetIndexer().Has(objId.Literal());
			}
			auto Find(Core::ObjectId id)
			{
				auto opaque = GetPool<OpaqueTrait>().Find(id.Literal());			
				if(opaque) return opaque;

				auto alpha = GetPool<AlphaBlendTrait>().Find(id.Literal());				
				if(alpha) return alpha;

				auto transparency = GetPool<TransparencyTrait>().Find(id.Literal());
				return transparency;
			}

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
			bool ChangeTrait(Core::ObjectId id)
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
			std::tuple<MeshPool<OpaqueTrait>, MeshPool<AlphaBlendTrait>, MeshPool<TransparencyTrait>> _tuple;
		};
	}
}