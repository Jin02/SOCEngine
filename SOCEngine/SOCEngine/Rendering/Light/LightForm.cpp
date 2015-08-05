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
	scene->GetLightManager()->Add(this, _owner->GetName().c_str());
}

void LightForm::OnDestroy()
{
}