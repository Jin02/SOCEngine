#include "CommonLightingBuffer.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

CommonLightingBuffer::CommonLightingBuffer()
  : _colorBuffer(nullptr), _optionalParamIndexBuffer(nullptr)
{
}

CommonLightingBuffer::~CommonLightingBuffer()
{
	SAFE_DELETE(_colorBuffer);
	SAFE_DELETE(_optionalParamIndexBuffer);
}

void CommonLightingBuffer::Initialize(uint count)
{
	const __int32 dummy[SPOT_LIGHT_BUFFER_MAX_NUM] = { 0, };
	_colorBuffer				= ColorBuffer::Create(count, DXGI_FORMAT_R8G8B8A8_UNORM, dummy);
	_optionalParamIndexBuffer	= OptionalParamIndexBuffer::Create(count, DXGI_FORMAT_R32_UINT, dummy);	
}

void CommonLightingBuffer::Destroy()
{
	_colorBuffer->Destroy();
	_optionalParamIndexBuffer->Destroy();
}

void CommonLightingBuffer::UpdateBuffer(const Light::LightForm* light,
				  const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
				  const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
				  bool existElem)
{
	address key = reinterpret_cast<address>(light);
	
	// Compute Optional Param Index
	ushort	shadowIndex			= getShadowIndex(light);
	uchar	lightShaftIndex		= getLightShaftIndex(light) & 0x7f;
	uchar	lightFlag			= light->GetFlag();
	uint	optionalParamIndex	= (shadowIndex << 16) | (lightFlag << 8) | lightShaftIndex;	
	uint	uintColor			= light->Get32BitMainColor();
	
	if(existElem)
	{
		(*_colorBuffer->Find(key))				= uintColor;
		(*_optionalParamIndexBuffer->Find(key))	= optionalParamIndex;
	}
	else
	{
		_colorBuffer->Add(key, uintColor);
		_optionalParamIndexBuffer->Add(key, optionalParamIndex);
	}
}

void CommonLightingBuffer::UpdateSRBuffer(ID3D11DeviceContext* context)
{
	_colorBuffer->UpdateSRBuffer(context);
	_optionalParamIndexBuffer->UpdateSRBuffer(context);
}

void CommonLightingBuffer::Delete(const Light::LightForm* light)
{
	address key = reinterpret_cast<address>(light);
	
	_colorBuffer->Delete(key);
	_optionalParamIndexBuffer->Delete(key);	
}

void CommonLightingBuffer::DeleteAll()
{
	_colorBuffer->DeleteAll();
	_optionalParamIndexBuffer->DeleteAll();
}