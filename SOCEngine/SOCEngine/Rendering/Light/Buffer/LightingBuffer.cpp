#include "CommonLightingBuffer.h"

using namespace Rednering;
using namespace Rednering::Light;

LightingBuffer::LightingBuffer()
  : _colorBuffer(nullptr), _optionalParamIndexBuffer(nullptr)
{
}

LightingBuffer::~LightingBuffer()
{
	SAFE_DELETE(_colorBuffer);
	SAFE_DELETE(_optionalParamIndexBuffer);
}

void LightingBuffer::Initialize(uint count)
{
	_colorBuffer			= ColorBuffer::Create(count,			DXGI_FORMAT_R8G8B8A8_UNORM,	dummyData);
	_optionalParamIndexBuffer	= OptionalParamIndexBuffer::Create(count,	DXGI_FORMAT_R32_UINT,		dummyData);	
}

void LightingBuffer::Destroy()
{
	_colorBuffer->Destroy();
	_optionalParamIndexBuffer->Destroy();
}

void LightingBuffer::UpdateBuffer(const Light::LightForm* light,
				  const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
				  const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
				  bool existElem)
{
	address key = reinterpret_cat<address>(light);
	
	// Compute Optional Param Index
	ushort	shadowIndex		= getShadowIndex(light);
	uchar	lightShaftIndex		= getLightShaftIndex(light) & 0x7f;
	uchar	lightFlag		= light->GetFlag();
	uint	optionalParamIndex	= (shadowIndex << 16) | (lightFlag << 8) | lightShaftIndex;
	
	uint	uintColor		= light->Get32BitMainColor();
	
	if(existElem)
	{
		(*_colorBuffer->Find(key))		= uintColor;
		(*_optionalParamIndexBuffer->Find(key))	= optionalParamIndex;
	}
	else
	{
		_colorBuffer->Add(key, uintColor);
		_optionalParamIndexBuffer->Add(key, optionalParamIndex);
	}
}

void LightingBuffer::UpdateSRBuffer(ID3D11DeviceContext* context)
{
	_colorBuffer->UpdateSRBuffer(context);
	_optionalParamIndexBuffer->UpdateSRBuffer(context);
}

void LightingBuffer::Delete(const Light::LightForm* light)
{
	address key = reinterpret_cast<address>(light);
	
	_colorBuffer->Delete(key);
	_optionalParamIndexBuffer->Delete(key);	
}

void LightingBuffer::DeleteAll()
{
	_colorBuffer->DeleteAll();
	_optionalParamIndexBuffer->DeleteAll();
}
