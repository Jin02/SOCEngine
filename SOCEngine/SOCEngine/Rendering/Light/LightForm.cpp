#include "LightForm.h"
#include "Object.h"

#include "Director.h"

using namespace Math;
using namespace Rendering;
using namespace Rendering::Light;

LightForm::LightForm()
	: _radius(10.0f), _lumen(500), _useShadow(false)
{
	_color			= Color::White();
}

LightForm::~LightForm()
{
}

void LightForm::OnInitialize()
{
	Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();

	const std::string& key = _owner->GetName();
	scene->GetLightManager()->Add(this, key.c_str());
}

uint LightForm::Get32BitMainColor() const
{
	uint uintColor = _color.Get32BitUintColor();
	
	uint packedLumen = (uint)((float)_lumen / (float)MAXIMUM_LUMEN * 255.0f + 0.5f);
	uintColor = (uintColor & 0x00ffffff) | (packedLumen << 24);

	return uintColor;
}

void LightForm::OnDestroy()
{
	if(_useShadow)
	{
		Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
		Shadow::ShadowRenderer* shadowManager = scene->GetShadowManager();

		const LightForm* light = this;
		shadowManager->DeleteShadowCastingLight(light);
	}
}

void LightForm::SetIntensity(float intensity)
{
	_lumen = (uint)(intensity * (float)(MAXIMUM_LUMEN / 5.0f));

	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}

float LightForm::GetIntensity() const
{
	return (float)_lumen * 5.0f / ((float)MAXIMUM_LUMEN);
}

void LightForm::SetRadius(float r)
{
	_radius = r;

	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}

void LightForm::SetColor(const Color& c)
{
	_color = c;

	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}

void LightForm::SetLumen(uint l)
{
	_lumen = l;

	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}

void LightForm::ActiveShadow(bool isActive)
{
	Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
	Shadow::ShadowRenderer* shadowManager = scene->GetShadowManager();

	const LightForm* light = this;

	if(isActive)
		shadowManager->AddShadowCastingLight(light);
	else
		shadowManager->DeleteShadowCastingLight(light);

	_useShadow = isActive;
}