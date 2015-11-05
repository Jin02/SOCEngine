#include "ShadowCommon.h"
#include "LightForm.h"
#include "Director.h"
#include "MathCommon.h"

using namespace Math;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Core;
using namespace Rendering;

ShadowCommon::ShadowCommon(const LightForm* owner, const std::function<void()>& ownerUpdateCounter) 
	: _owner(owner), _bias(0.001f), _ownerAddUpdateCounter(ownerUpdateCounter)
{
	_color = Color::Black();
	_color.a = 0.5f;
}

ShadowCommon::~ShadowCommon()
{
}

void ShadowCommon::Initialize()
{
}

void ShadowCommon::MakeParam(CommonParam& outParam) const
{
	outParam.bias		= _bias;//Common::FloatToHalf(_bias);
	outParam.index		= FetchShadowCastingLightIndex() + 1;//Common::FloatToHalf( (float)FetchShadowCastingLightIndex() );
}

ushort ShadowCommon::FetchShadowCastingLightIndex() const
{
	Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
	ShadowRenderer* shadowManager = scene->GetShadowManager();

	const LightForm* owner = _owner;
	return shadowManager->FetchShadowCastingLightIndex(owner);
}

void ShadowCommon::SetBias(float bias)
{
	_bias = bias;
	_ownerAddUpdateCounter();
}

void ShadowCommon::SetColor(const Color& color)
{
	_color = color;
	_ownerAddUpdateCounter();
}