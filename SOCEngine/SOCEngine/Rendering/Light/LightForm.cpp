#include "LightForm.h"
#include "Object.h"

#include "Director.h"

using namespace Rendering;
using namespace Rendering::Light;

LightForm::LightForm()
	: _radius(10.0f), _lumen(500)
{
	_color = Color::White();
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

uint LightForm::GetShderUintColor() const
{
	uint uintColor = _color.Get32BitUintColor();
	
	uint packedLumen = (uint)((float)_lumen / (float)MAXIMUM_LUMEN * 255.0f + 0.5f);
	uintColor = (uintColor & 0x00ffffff) | (packedLumen << 24);

	return uintColor;
}

void LightForm::OnDestroy()
{
}