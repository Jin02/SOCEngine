#include "LightForm.h"
#include "Object.h"

#include "Director.h"

using namespace Rendering;
using namespace Rendering::Light;

LightForm::LightForm()
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

void LightForm::OnDestroy()
{
}