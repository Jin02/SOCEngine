#include "SpotLightingBuffer.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

SpotLightingBuffer::SpotLightingBuffer()
	: _transformBuffer(nullptr), _paramBuffer(nullptr), _commonBuffer()
{
}

SpotLightingBuffer::~SpotLightingBuffer()
{
	Destroy();
	
	SAFE_DELETE(_transformBuffer);
	SAFE_DELETE(_paramBuffer);
}

void SpotLightingBuffer::Initialize()
{
	const __int32 dummyData[SPOT_LIGHT_BUFFER_MAX_NUM * sizeof(LightForm::LightTransformBuffer) / 4] = {0, };
	
	_transformBuffer	= TransformBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM,	DXGI_FORMAT_R32G32B32A32_FLOAT,		dummyData);
	_paramBuffer		= ParamBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM, 	DXGI_FORMAT_R16G16B16A16_FLOAT,		dummyData);

	_commonBuffer.Initialize(SPOT_LIGHT_BUFFER_MAX_NUM);
}

void SpotLightingBuffer::Destroy()
{
	_transformBuffer->Destroy();
	_paramBuffer->Destroy();
	
	_commonBuffer.Destroy();
}

bool SpotLightingBuffer::UpdateBuffer(	const LightWithPrevUpdateCounter& lightWithPrevUC,
					const std::function<uchar(const LightForm*)>& getShadowIndex,
					const std::function<uchar(const LightForm*)>& getLightShaftIndex)
{
	auto _UpdateBuffer = [&](const LightForm* light) -> void
	{
		address key = reinterpret_cat<address>(light);
		
		LightForm::LightTransformBuffer	transform;
		SpotLight::Param		param;
		static_cast<const SpotLight*>(light)->MakeParam(transform, param);
								
		// 하나만 검색해도 됨
		LightForm::LightTransformBuffer* existTarnsform = _transformBuffer->Find(key);
		if( existTarnsform == nullptr )
		{
			_transformBuffer->Add(key, transform);
			_paramBuffer->Add(key, param);
		}
		else // existTarnsform != nullptr
		{
			(*existTarnsform)		= transform;
			(*_paramBuffer->Find(key))	= param;
		}
		
		_commonBuffer.UpdateBuffer(light, getShadowIndex, getLightShaftIndex, existTarnsform != nullptr);
	};
	
	return lightWithPrevUC->UpdateBuffer(_UpdateBuffer);
}

void SpotLightingBuffer::UpdateSRBuffer(ID3D11DeviceContext* context,
					const LightWithPrevUpdateCounter& lightWithPrevUC,
					const std::function<uchar(const LightForm*)>& getShadowIndex,
					const std::function<uchar(const LightForm*)>& getLightShaftIndex,
				   	bool forcedUpdate)
{
	bool isNeedToUpdate = UpdateBuffer(lightWithPrevUC, getShadowIndex, getLightShaftIndex);

	if((isNeedToUpdate || forcedUpdate) == false)
		return;
	
	_transformBuffer->UpdateSRBuffer(context);
	_paramBuffer->UpdateSRBuffer(context);
	
	_commonBuffer.UpdateSRBuffer(context);
}

void SpotLightingBuffer::Delete(const SpotLight* light)
{
	address key = reinterpret_cast<address>(light);
	
	_transformBuffer->Delete(key);
	_paramBuffer->Delete(key);
	
	_commonBuffer.Delete(key);
}
