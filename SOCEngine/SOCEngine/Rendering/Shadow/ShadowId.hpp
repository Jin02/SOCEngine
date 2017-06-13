#pragma once

#include "UniqueIdManager.hpp"
#include "UniqueId.hpp"

namespace Rendering
{
	namespace Shadow
	{
		class ShadowId : public Core::UniqueId
		{
		public:
			using Core::UniqueId::UniqueId;

		public:
			friend class ShadowIdManager;
		};

		class ShadowIdManager final : public Core::IdManagerForm<ShadowId> {};
	}
}