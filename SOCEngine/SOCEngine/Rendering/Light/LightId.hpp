#pragma once

#include "UniqueIdManager.hpp"
#include "UniqueId.hpp"

namespace Rendering
{
	namespace Light
	{
		class LightId : public Core::UniqueId
		{
		public:
			using Core::UniqueId::UniqueId;

		public:
			friend class LightIdManager;
		};

		class LightIdManager final : public Core::IdManagerForm<LightId> {};
	}
}