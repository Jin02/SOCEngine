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
			using UniqueId::UniqueId;

		public:
			friend class LightIdManager;
		};

		class LightIdManager : public Core::UniqueIdManager
		{
		public:
			LightId Acquire()
			{
				uint literalId = UniqueIdManager::Acquire();
				return LightId(literalId);
			}

			bool Has(LightId id)
			{
				return UniqueIdManager::Has(id.Literal());
			}

			void Delete(LightId id)
			{
				UniqueIdManager::Delete(id.Literal());
			}
		};
	}
}