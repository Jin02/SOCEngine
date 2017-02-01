#pragma once

#include "GPUUploadBuffer.h"
#include "LightWithPrevUpdateCounter.h"
#include "CommonLightingBuffer.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class SpotLightingBuffer : public LightingBufferForm<SpotLight>
			{
			private:
				typedef LightingBufferForm<SpotLight> 					Parent;
				
				typedef Buffer::GPUUploadBuffer<address, SpotLight::Param>		ParamBuffer;
				ParamBuffer*								_paramBuffer;
				
			public:
				SpotLightingBuffer();
				~SpotLightingBuffer();
				
			public:
				void Initialize();
				void Destroy();

			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount);
/*
				virtual void UpdateAdditionalBuffer(const LightType* light, bool existElem) {}
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) {}
				virtual void OnDelete(const LightType* light) { }
				virtual void OnDeleteAll() {}
*/
			};
		}
	}
}
