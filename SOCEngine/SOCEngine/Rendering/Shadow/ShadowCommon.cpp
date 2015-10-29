#include "ShadowCommon.h"
#include "LightForm.h"
#include "Director.h"
#include "MathCommon.h"

using namespace Math;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Core;

ShadowCommon::ShadowCommon(const LightForm* owner) 
	: _owner(owner), _bias(0.5f)
{
	_color = Color::Black();
}

ShadowCommon::~ShadowCommon()
{
}

void ShadowCommon::MakeParam(CommonParam& outParam) const
{
	CommonParam param;
	param.r			= Common::FloatToHalf(_color.r);
	param.g			= Common::FloatToHalf(_color.g);
	param.b			= Common::FloatToHalf(_color.b);
	param.strength	= Common::FloatToHalf(_color.a);

	param.bias		= Common::FloatToHalf(_bias);
	param.index		= Common::FloatToHalf( (float)FetchShadowCastingLightIndex() );

	outParam = param;
}

ushort ShadowCommon::FetchShadowCastingLightIndex() const
{
	Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
	ShadowRenderer* shadowManager = scene->GetShadowManager();

	const LightForm* owner = _owner;
	return shadowManager->FetchShadowCastingLightIndex(owner);
}