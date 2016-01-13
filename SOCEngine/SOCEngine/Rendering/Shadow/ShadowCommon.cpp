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
	: _owner(owner), _bias(0.001f), _projNear(0.1f),
	_paramUpdateCounter(0), _transformUpdateCounter(0), _useVSM(false)
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

void ShadowCommon::MakeParam(CommonParam& outParam, uint lightIndex) const
{
	uint index	= lightIndex << 16;
	uint bias	= (uint(_bias * 4096.0f) & 0x0fff) << 4;
	uint flag	= uint(_useVSM) & 0xf;

	outParam.lightIndexWithBiasWithFlag = index | bias | flag;
	outParam.color						= _color.Get32BitUintColor();
}

void ShadowCommon::SetBias(float bias)
{
	_bias = bias;
	++_paramUpdateCounter;
}

void ShadowCommon::SetUseVSM(bool b)
{
	_useVSM = b;
	++_paramUpdateCounter;
}

void ShadowCommon::SetColor(const Color& color)
{
	_color = color;
	++_paramUpdateCounter;
}

void ShadowCommon::SetProjNear(float n)
{
	_projNear = n;
	++_transformUpdateCounter;
}