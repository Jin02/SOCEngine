#include "SpotLightingBuffer.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

SpotLightingBuffer::SpotLightingBuffer()
	: Parent(), _paramBuffer(nullptr)
{
}

SpotLightingBuffer::~SpotLightingBuffer()
{
	SAFE_DELETE(_paramBuffer);
}

void SpotLightingBuffer::Initialize()
{
	const __int32 dummyData[SPOT_LIGHT_BUFFER_MAX_NUM * sizeof(SpotLight::Param) / 4] = {0, };
	_paramBuffer = ParamBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM,  DXGI_FORMAT_R16G16B16A16_FLOAT, dummyData);
	
	Parent::Initialize(SPOT_LIGHT_BUFFER_MAX_NUM);
}

void SpotLightingBuffer::Destroy()
{
	_paramBuffer->Destroy();
	Parent::Destroy();
}

void SpotLightingBuffer::UpdateAdditionalBuffer(const SpotLight* light, bool existElem)
{
	address key = reinterpret_cast<address>(light);
	
	SpotLight::Param param;
	light->MakeParam(param);
	
	if(existElem)		(*_paramBuffer->Find(key)) = param;
	else			_paramBuffer->Add(key, param);
}

void SpotLightingBuffer::UpdateAdditionalSRBuffer(ID3D11DeviceContext* context)
{
	_paramBuffer->UpdateSRBuffer(context);
}

void SpotLightingBuffer::OnDelete(const SpotLight* light)
{
	address key = reinterpret_cast<address>(light);
	_paramBuffer->Delete(key);
}

void SpotLightingBuffer::OnDeleteAll()
{
	_paramBuffer->DeleteAll();
}

TransformBuffer* SpotLightingBuffer::InitializeTransformBuffer(uint maxLightCount)
{
	const __int32 dummy[maxLightCount * sizeof(Parent::TransformType) / 4] = {0, };
	return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R32G32B32A32_FLOAT, dummy);
}

SpotLightingBuffer* SpotLightingBuffer::Create()
{
	SpotLightingBuffer* buff = new SpotLightingBuffer;
	buff->Initialize();
	
	return buff;
}
