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
			class SpotLightingBuffer
			{
			private:
				typedef Buffer::GPUUploadBuffer<address, LightForm::LightTransformBuffer>	TransformBuffer;
				typedef Buffer::GPUUploadBuffer<address, Light::SpotLight::Param>		ParamBuffer;

				TransformBuffer*				_transformBuffer;
				ParamBuffer*					_paramBuffer;
				CommonLightingBuffer				_commonBuffer;
				
			public:
				SpotLightingBuffer();
				~SpotLightingBuffer();
				
			public:
				void Initialize();
				void Destroy();
				
			private:
				bool UpdateBuffer(const LightWithPrevUpdateCounter& lightWithPrevUC,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);	

				public:
				void UpdateSRBuffer(	ID3D11DeviceContext* context,
							const LightWithPrevUpdateCounter& lightWithPrevUC,
							const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
							const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
							bool forcedUpdate = false	);				
				void Delete(const Light::SpotLight* light);
			};
		}
	}
}
