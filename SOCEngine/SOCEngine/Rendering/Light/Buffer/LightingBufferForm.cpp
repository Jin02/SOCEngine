#include "LightingBufferForm.h"

using namespace Rednering;
using namespace Rednering::Light;
using namespace Rendering::Light::LightingBuffer;

LightingBufferForm::LightingBufferForm()
  : _colorBuffer(nullptr), _optionalParamIndexBuffer(nullptr)
{
}

LightingBufferForm::~LightingBufferForm()
{
	SAFE_DELETE(_colorBuffer);
	SAFE_DELETE(_optionalParamIndexBuffer);
}

void LightingBufferForm::Initialize(uint count)
{
	_colorBuffer			= ColorBuffer::Create(count,			DXGI_FORMAT_R8G8B8A8_UNORM,	dummyData);
	_optionalParamIndexBuffer	= OptionalParamIndexBuffer::Create(count,	DXGI_FORMAT_R32_UINT,		dummyData);	
}

void LightingBufferForm::Destroy()
{
	_colorBuffer->Destroy();
	_optionalParamIndexBuffer->Destroy();
}

void LightingBufferForm::UpdateBuffer(const Light::LightForm* light,
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

void LightingBufferForm::UpdateSRBuffer(ID3D11DeviceContext* context)
{
	_colorBuffer->UpdateSRBuffer(context);
	_optionalParamIndexBuffer->UpdateSRBuffer(context);
}

void LightingBufferForm::Delete(const Light::LightForm* light)
{
	address key = reinterpret_cast<address>(light);
	
	_colorBuffer->Delete(key);
	_optionalParamIndexBuffer->Delete(key);	
}

void LightingBufferForm::DeleteAll()
{
	_colorBuffer->DeleteAll();
	_optionalParamIndexBuffer->DeleteAll();
}
