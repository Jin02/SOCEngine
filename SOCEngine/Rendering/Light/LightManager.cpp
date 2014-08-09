#include "LightManager.h"
#include "Object.h"
#include "EngineMath.h"


using namespace Math;
using namespace std;
using namespace Rendering;

namespace Rendering
{
	namespace Light
	{
		LightManager::LightManager(void)
		{
		}

		LightManager::~LightManager(void)
		{
		}

		bool LightManager::Intersects(std::vector<LightForm*>& outLights, Frustum *frustum)
		{
			bool intersect = false;

			for(auto iter = _lights.begin(); iter != _lights.end(); ++iter)
			{
				Vector3 worldP;
				(*iter)->GetOwner()->GetTransform()->WorldPosition(worldP);
				float radius = (*iter)->range;
				
				if((*iter)->GetLightType() == LightForm::LightType::Directional)
				{
					outLights.push_back(*iter);
					intersect = true;
				}
				else if( frustum->In( worldP, radius ) )
				{
					outLights.push_back( *iter );
					intersect = true;
				}
			}

			return intersect;
		}

		bool LightManager::Intersects(Frustum *frustum, const LightForm* light)
		{
			bool intersect = false;

			Vector3 worldP;
			light->GetOwner()->GetTransform()->WorldPosition(worldP);
			float radius = light->range;

			if( frustum->In( worldP, radius ) )
				intersect = true;

			return intersect;
		}

		bool LightManager::AddLight(LightForm* light)
		{
			_lights.push_back(light);
			return true;
		}

		void LightManager::Delete(LightForm* light, bool remove)
		{
			for(auto iter = _lights.begin(); iter != _lights.end(); ++iter)
			{
				if( (*iter) == light )
				{
					if(remove)
						delete (*iter);

					_lights.erase(iter);
					return;
				}
			}
		}

		void LightManager::DeleteAll(bool remove)
		{
			if(remove)
			{
				for(auto iter = _lights.begin(); iter != _lights.end(); ++iter)
					SAFE_DELETE((*iter));
			}

			_lights.clear();
		}
	}
}