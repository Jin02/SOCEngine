#include "ShadowCommon.h"
#include "LightForm.h"
#include "Director.h"
#include "MathCommon.h"

using namespace Math;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Core;
using namespace Rendering;

ShadowCommon::ShadowCommon(const LightForm* owner) 
	: _owner(owner),
	_projNear(0.1f), _projFar(32.0f), _underScanSize(4.25f), _softness(1.0f), _bias(0.001f), _flag(0),
	_paramUpdateCounter(0), _useVSM(false)
{
	_color = Color::Black();
	_color.a = 0.8f;
}

ShadowCommon::~ShadowCommon()
{
}

void ShadowCommon::Initialize()
{
}

void ShadowCommon::MakeParam(Param& outParam, uint lightIndex) const
{
	outParam.projNear		= Math::Common::FloatToHalf(_projNear);
	outParam.projFar		= Math::Common::FloatToHalf(_projFar);

	outParam.underScanSize	= Math::Common::FloatToHalf(_underScanSize);
	outParam.softness		= Math::Common::FloatToHalf(_softness);

	outParam.lightIndex		= lightIndex;
	outParam.bias			= uchar(_bias * 1020.0f);
	outParam.flag			= uchar(_useVSM);

	outParam.shadowColor	= _color.Get32BitUintColor();
}

void ShadowCommon::SetProjectionNear(float n)
{
	_projNear = n;
	_owner->GetOwner()->GetTransform()->AddUpdateCounter();
}