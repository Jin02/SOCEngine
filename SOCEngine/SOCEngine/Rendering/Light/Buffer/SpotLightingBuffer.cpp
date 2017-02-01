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

/*
				virtual void UpdateAdditionalBuffer(const LightType* light, bool existElem) {}
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) {}
				virtual void OnDelete(const LightType* light) { }
				virtual void OnDeleteAll() {}
*/

TransformBuffer* SpotLightingBuffer::InitializeTransformBuffer(uint maxLightCount)
{
	const __int32 dummy[maxLightCount * sizeof(Parent::TransformType) / 4] = {0, };
	return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R32G32B32A32_FLOAT, dummy);
}
