#pragma once

#include "LightForm.h"
#include "Utility.h"
#include "Frustum.h"

using namespace std;

namespace Rendering
{
	namespace Light
	{
		class LightManager
		{
		private:
			std::vector<LightForm*> lights;

		public:
			LightManager(void);
			~LightManager(void);

		public:
			bool Intersect(Frustum *frustum, std::vector<LightForm*> *out);
			bool Intersect(Frustum *frustum, LightForm* light);

		public:
			bool AddLight(LightForm* light);
			void Delete(LightForm* light, bool remove);
			void DeleteAll(bool remove);
		};
	}
}