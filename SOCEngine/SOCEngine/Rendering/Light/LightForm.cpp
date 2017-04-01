#include "LightForm.h"
#include "GlobalDefine.h"

using namespace Core;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Light;

BaseLight::BaseLight(ObjectId id)
	: _id(id)
{
}

void BaseLight::SetIntensity(float intensity)
{
	_lumen = static_cast<uint>(intensity * static_cast<double>(MAXIMUM_LUMEN / 8.0));
	_dirty = true;
}

float BaseLight::GetIntensity() const
{
	return static_cast<float>( static_cast<double>(_lumen) * 8.0f / MAXIMUM_LUMEN );
}

uint BaseLight::Get32BitMainColor() const
{
	uint uintColor = _color.Get32BitUintColor();

	uint packedLumen = static_cast<uint>(static_cast<double>(_lumen) / MAXIMUM_LUMEN * 255.0 + 0.5);
	uintColor = (uintColor & 0x00ffffff) | (packedLumen << 24);

	return uintColor;
}