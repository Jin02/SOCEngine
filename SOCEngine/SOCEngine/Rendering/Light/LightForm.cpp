#include "LightForm.h"
#include "Object.h"

#include "Director.h"

using namespace Rendering;
using namespace Rendering::Light;

LightForm::LightForm()
	: _radius(10.0f), _lumen(500), _useShadow(false)
{
	_color			= Color::White();
	_shadowColor	= Color::Black();
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

void LightForm::SetShadowColor(const Color& c)
{
	_shadowColor = c;
	
	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}

uint LightForm::Get32BitShadowColor() const
{
	uint uintColor = _shadowColor.Get32BitUintColor();	
	uintColor = (_useShadow ? (uintColor & 0xff000000) : 0) | (uintColor & 0x00ffffff);

	return uintColor;
}