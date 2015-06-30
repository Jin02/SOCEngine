#include "LightManager.h"
#include "Object.h"
#include "EngineMath.h"
#include "Utility.h"

using namespace Math;
using namespace std;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Camera;

LightManager::LightManager(void)
{
}

LightManager::~LightManager(void)
{
}

void LightManager::Add(const LightForm* light, const char* key)
{
	std::string searchKey = key ? key : light->GetOwner()->GetName();
	LightForm::LightType lightType = light->GetType();
	uint uintColor = light->GetColor().Get32BitUintColor();

	if(lightType == LightForm::LightType::Directional)
	{
		const DirectionalLight* l = dynamic_cast<const DirectionalLight*>(light);

		LightForm::LightTransformBuffer transformElem;
		DirectionalLight::Params param;

		l->MakeLightBufferElement(transformElem, param);

		_directionalLightTransformBuffer.Add(searchKey, transformElem);
		_directionalLightParammBuffer.Add(searchKey, param);
		_directionalLightColorBuffer.Add(searchKey, uintColor);
	}
	else if(lightType == LightForm::LightType::Point)
	{
		const PointLight* l = dynamic_cast<const PointLight*>(light);

		LightForm::LightTransformBuffer transformElem;
		l->MakeLightBufferElement(transformElem);

		_pointLightTransformBuffer.Add(searchKey, transformElem);
		_pointLightColorBuffer.Add(searchKey, uintColor);
	}
	else if(lightType == LightForm::LightType::Spot)
	{
		const SpotLight* l = dynamic_cast<const SpotLight*>(light);

		LightForm::LightTransformBuffer transformElem;
		SpotLight::Params param;

		l->MakeLightBufferElement(transformElem, param);

		_spotLightTransformBuffer.Add(searchKey, transformElem);
		_spotLightParamBuffer.Add(searchKey, param);
		_spotLightColorBuffer.Add(searchKey, uintColor);
	}
}

void LightManager::Delete(const std::string& key, LightForm::LightType type)
{
	if(type == LightForm::LightType::Directional)
	{
		_directionalLightColorBuffer.Delete(key);
		_directionalLightTransformBuffer.Delete(key);
		_directionalLightParammBuffer.Delete(key);
	}
	else if(type == LightForm::LightType::Point)
	{
		_pointLightTransformBuffer.Delete(key);
		_pointLightColorBuffer.Delete(key);
	}
	else if(type == LightForm::LightType::Spot)
	{
		_spotLightTransformBuffer.Delete(key);
		_spotLightColorBuffer.Delete(key);
		_spotLightParamBuffer.Delete(key);
	}
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
}