#include "LightManager.h"
#include "Object.h"
#include "EngineMath.h"
#include "Utility.h"

using namespace Math;
using namespace std;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Camera;

LightManager::LightManager(void) : _directionalLightUpdateCounter(0), _spotLightUpdateCounter(0), _pointLightUpdateCounter(0)
{
}

LightManager::~LightManager(void)
{
}

void LightManager::Add(const LightForm* light, const char* key)
{
	std::string searchKey = key ? key : light->GetOwner()->GetName();

	ASSERT_COND_MSG(HasKey(searchKey), "Already has Key");

	//1을 더하는건, UpdateBuffer에서 비교할때 버그 안생기게 하려고 -ㅠ-..
	uint counter = light->GetOwner()->GetTransform()->GetUpdateCounter() + 1;
	_lights.Add(searchKey, Lights(light, counter));
}

void LightManager::UpdateBuffer()
{
	auto& lights					= _lights.GetVector();
	bool isChangedDirectionalLight	= false;
	bool isChangedSpotLight			= false;
	bool isChangedPointLight		= false;

	uint allLightBufferIdx = 0;
	for(auto iter = lights.begin(); iter != lights.end(); ++iter, ++allLightBufferIdx)
	{
		const Light::LightForm* light = iter->light;

		uint& prevTransformUpdateCounter = _lights.Get(allLightBufferIdx).prevTransformUpdateCounter;
		uint currentUpdateCounter = light->GetOwner()->GetTransform()->GetUpdateCounter();

		if(currentUpdateCounter != prevTransformUpdateCounter)
		{
			std::string key = light->GetOwner()->GetName();

			LightForm::LightType lightType = light->GetType();
			uint uintColor = light->GetColor().Get32BitUintColor();

			if(lightType == LightForm::LightType::Directional)
			{
				const DirectionalLight* l = dynamic_cast<const DirectionalLight*>(light);

				LightForm::LightTransformBuffer transformElem;
				DirectionalLight::Params param;

				l->MakeLightBufferElement(transformElem, param);

				if( _directionalLightTransformBuffer.Find(key) == nullptr ) //하나만 검색해도 됨
				{
					_directionalLightTransformBuffer.Add(key, transformElem);
					_directionalLightParammBuffer.Add(key, param);
					_directionalLightColorBuffer.Add(key, uintColor);
				}
				else
				{
					(*_directionalLightTransformBuffer.Find(key)) = transformElem;
					(*_directionalLightParammBuffer.Find(key)) = param;
					(*_directionalLightColorBuffer.Find(key)) = uintColor;
				}
				isChangedDirectionalLight = true;
			}

			else if(lightType == LightForm::LightType::Point)
			{
				const PointLight* l = dynamic_cast<const PointLight*>(light);

				LightForm::LightTransformBuffer transformElem;
				l->MakeLightBufferElement(transformElem);

				if(_pointLightTransformBuffer.Find(key) == nullptr)
				{
					_pointLightTransformBuffer.Add(key, transformElem);
					_pointLightColorBuffer.Add(key, uintColor);
				}
				else
				{
					(*_pointLightTransformBuffer.Find(key)) = transformElem;
					(*_pointLightColorBuffer.Find(key)) = uintColor;
				}
				isChangedPointLight = true;
			}

			else if(lightType == LightForm::LightType::Spot)
			{
				const SpotLight* l = dynamic_cast<const SpotLight*>(light);

				LightForm::LightTransformBuffer transformElem;
				SpotLight::Params param;

				l->MakeLightBufferElement(transformElem, param);

				if(_spotLightTransformBuffer.Find(key) == nullptr)
				{
					_spotLightTransformBuffer.Add(key, transformElem);
					_spotLightParamBuffer.Add(key, param);
					_spotLightColorBuffer.Add(key, uintColor);
				}
				else
				{
					(*_spotLightTransformBuffer.Find(key)) = transformElem;
					(*_spotLightParamBuffer.Find(key)) = param;
					(*_spotLightColorBuffer.Find(key)) = uintColor;
				}
				isChangedSpotLight = true;			
			}

			prevTransformUpdateCounter = currentUpdateCounter;
		}
	}

	if(isChangedDirectionalLight)	++_directionalLightUpdateCounter;
	if(isChangedPointLight)			++_pointLightUpdateCounter;
	if(isChangedSpotLight)			++_spotLightUpdateCounter;
}

void LightManager::Delete(const std::string& key)
{
	const Light::LightForm* light = _lights.Find(key)->light;
	Light::LightForm::LightType type = light->GetType();

	if(type == LightForm::LightType::Directional)
	{
		_directionalLightColorBuffer.Delete(key);
		_directionalLightTransformBuffer.Delete(key);
		_directionalLightParammBuffer.Delete(key);

		++_directionalLightUpdateCounter;
	}
	else if(type == LightForm::LightType::Point)
	{
		_pointLightTransformBuffer.Delete(key);
		_pointLightColorBuffer.Delete(key);

		++_pointLightUpdateCounter;
	}
	else if(type == LightForm::LightType::Spot)
	{
		_spotLightTransformBuffer.Delete(key);
		_spotLightColorBuffer.Delete(key);
		_spotLightParamBuffer.Delete(key);

		++_spotLightUpdateCounter;
	}

	_lights.Delete(key);
}

void LightManager::DeleteAll()
{
	_spotLightTransformBuffer.DeleteAll();
	_spotLightColorBuffer.DeleteAll();
	_spotLightParamBuffer.DeleteAll();

	_pointLightTransformBuffer.DeleteAll();
	_pointLightColorBuffer.DeleteAll();
	
	_directionalLightColorBuffer.DeleteAll();
	_directionalLightParammBuffer.DeleteAll();
	_directionalLightTransformBuffer.DeleteAll();

	_lights.DeleteAll();

	_directionalLightUpdateCounter = 
	_spotLightUpdateCounter = 
	_pointLightUpdateCounter = 0;
}

bool LightManager::HasKey(const std::string& key)
{
	return _lights.Find(key) != nullptr;
}