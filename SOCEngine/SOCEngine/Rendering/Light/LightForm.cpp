#include "LightForm.h"
#include "Object.h"

#include "Director.h"

using namespace Math;
using namespace Rendering;
using namespace Rendering::Light;

LightForm::LightForm()
	: _radius(10.0f), _lumen(500), _shadow(nullptr), _projNear(0.1f)
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

	LightForm* thisLight = this;
	scene->GetLightManager()->Add(thisLight);
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
	if(_shadow)
	{
		Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
		Shadow::ShadowRenderer* shadowManager = scene->GetShadowManager();

		const LightForm* light = this;
		shadowManager->DeleteShadowCastingLight(light);

		SAFE_DELETE(_shadow);
	}
}

void LightForm::SetIntensity(float intensity)
{
	_lumen = (uint)(intensity * (float)(MAXIMUM_LUMEN / 5.0f));
	AddOwnerUpdateCounter();
}

float LightForm::GetIntensity() const
{
	return (float)_lumen * 5.0f / ((float)MAXIMUM_LUMEN);
}

void LightForm::SetRadius(float r)
{
	_radius = r;
	AddOwnerUpdateCounter();
}

void LightForm::SetColor(const Color& c)
{
	_color = c;
	AddOwnerUpdateCounter();
}

void LightForm::SetLumen(uint l)
{
	_lumen = l;
	AddOwnerUpdateCounter();
}

void LightForm::ActiveShadow(bool isActive)
{
	Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
	Shadow::ShadowRenderer* shadowManager = scene->GetShadowManager();

	const LightForm* light = this;

	if(isActive)
	{
		shadowManager->AddShadowCastingLight(light);

		auto AddCounter = [&]()
		{
			AddOwnerUpdateCounter();
		};
		CreateLightShadow(AddCounter);
	}
	else
	{
		shadowManager->DeleteShadowCastingLight(light);
		SAFE_DELETE(_shadow);
	}
}

void LightForm::AddOwnerUpdateCounter()
{
	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}