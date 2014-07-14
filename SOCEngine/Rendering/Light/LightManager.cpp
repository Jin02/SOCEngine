#include "LightManager.h"
#include "Object.h"

namespace Rendering
{
	namespace Light
	{
		LightManager::LightManager(void)
		{
			lights.reserve(MAX_LIGHT);
		}

		LightManager::~LightManager(void)
		{
		}

		bool LightManager::Intersect(Frustum *frustum, std::vector<LightForm*> *out)
		{
			bool intersect = false;

			for(vector<LightForm*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
			{
				Vector3 worldP = (*iter)->GetWorldPosition();
				float radius = (*iter)->range;
				
				if((*iter)->GetType() == LightForm::LightType::DIRECTIONAL)
				{
					out->push_back(*iter);
					intersect = true;
				}
				else if( frustum->In( worldP, radius ) )
				{
					out->push_back( *iter );
					intersect = true;
				}
			}

			return intersect;
		}

		bool LightManager::Intersect(Frustum *frustum, LightForm* light)
		{
			bool intersect = false;

			for(vector<LightForm*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
			{
				Vector3 worldP = (*iter)->GetWorldPosition();
				float radius = (*iter)->range;
				if( frustum->In( worldP, radius ) )
				{
					intersect = true;
					break;
				}
			}

			return intersect;
		}

		bool LightManager::AddLight(LightForm* light)
		{
			lights.push_back(light);
			return true;
		}

		void LightManager::Delete(LightForm* light, bool remove)
		{
			for(vector<LightForm*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
			{
				if( (*iter) == light )
				{
					if(remove)
						delete (*iter);

					lights.erase(iter);
					return;
				}
			}
		}

		void LightManager::DeleteAll(bool remove)
		{
			if(remove)
			{
				for(vector<LightForm*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
					Utility::SAFE_DELETE((*iter));
			}

			lights.clear();
		}
	}
}