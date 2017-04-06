#include "CommonLightingBuffer.h"

using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

void CommonLightingBuffer::Initialize(Device::DirectX& dx, uint count)
{
	const __int32 dummy[2048]	= { 0, };
	_colorBuffer.Initialize(dx, count, DXGI_FORMAT_R8G8B8A8_UNORM, dummy);
	_optionalParamIndexBuffer.Initialize(dx, count, DXGI_FORMAT_R32_UINT, dummy);	
}

void CommonLightingBuffer::Destroy()
{
	_colorBuffer.Destroy();
	_optionalParamIndexBuffer.Destroy();
}

void CommonLightingBuffer::UpdateBuffer(const Light::BaseLight& light, uint shadowIndex, uint lightShaftIndex, bool existElem)
{	
	auto key = light.GetObjectId();

	// Compute Optional Param Index
	ushort	shadowIndex		= shadowIndex;
	uchar	lightShaftIndex		= lightShaftIndex & 0x7f;
	uchar	lightFlag		= light.GetFlag();
	uint	optionalParamIndex	= (shadowIndex << 16) | (lightFlag << 8) | lightShaftIndex;	
	uint	uintColor		= light.Get32BitMainColor();
	
	if(existElem)
	{
		(*_colorBuffer.Find(key)) = uintColor;
		(*_optionalParamIndexBuffer.Find(key)) = optionalParamIndex;
	}
	else
	{
		_colorBuffer.Add(key, uintColor);
		_optionalParamIndexBuffer.Add(key, optionalParamIndex);
	}
}

void CommonLightingBuffer::UpdateSRBuffer(Device::DirectX& dx)
{
	_colorBuffer.UpdateSRBuffer(dx);
	_optionalParamIndexBuffer.UpdateSRBuffer(dx);
}

void CommonLightingBuffer::Delete(KeyType key)
{
	_colorBuffer.Delete(key);
	_optionalParamIndexBuffer.Delete(key);	
}

void CommonLightingBuffer::DeleteAll()
{
	_colorBuffer.DeleteAll();
	_optionalParamIndexBuffer.DeleteAll();
}
