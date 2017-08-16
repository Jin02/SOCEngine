#pragma once

#include "UniqueIDManager.hpp"
#include "UniqueID.hpp"
#include "VectorIndexer.hpp"

namespace Rendering
{
	class MaterialID : public Core::UniqueID<MaterialID>
	{
	public:
		using Parent = Core::UniqueID<MaterialID>;
		using Parent::UniqueID;
		using Parent::Literal;

		friend class Core::IDManagerForm<MaterialID>;
	};

	using MaterialIDManager = Core::IDManagerForm<MaterialID>;
}