#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorHashMap.h"
#include "ShaderResourceBuffer.h"

#include <functional>

#define POINT_LIGHT_BUFFER_MAX_NUM			2048
#define SPOT_LIGHT_BUFFER_MAX_NUM			2048
#define DIRECTIONAL_LIGHT_BUFFER_MAX_NUM	1024
#define LIGHT_BUFFER_OVERALL_UPDATE_RATE	0.5f

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		public:
			struct Lights
			{
				uint				prevTransformUpdateCounter;
				Light::LightForm*	light;

				Lights(Light::LightForm* _light, uint prevTFCounter) : light(_light), prevTransformUpdateCounter(prevTFCounter) {}
				~Lights(){}
			};

		private:
			Structure::VectorHashMap<address, Lights>										_lights;
			Structure::VectorHashMap<address, Light::DirectionalLight*>						_directionalLights; // using for compute frustum

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Buffer::ShaderResourceBuffer*													_pointLightTransformSRBuffer;
			Structure::VectorHashMap<address, uint>											_pointLightColorBuffer;
			Buffer::ShaderResourceBuffer*													_pointLightColorSRBuffer;
			Structure::VectorHashMap<address, ushort>										_pointLightShadowIndexBuffer;
			Buffer::ShaderResourceBuffer*													_pointLightShadowIndexSRBuffer;

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_directionalLightTransformBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightTransformSRBuffer;
			Structure::VectorHashMap<address, Light::DirectionalLight::Param>				_directionalLightParamBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightParamSRBuffer;
			Structure::VectorHashMap<address, uint>											_directionalLightColorBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightColorSRBuffer;
			Structure::VectorHashMap<address, ushort>										_directionalLightShadowIndexBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightShadowIndexSRBuffer;

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightTransformSRBuffer;
			Structure::VectorHashMap<address, Light::SpotLight::Param>						_spotLightParamBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightParamSRBuffer;
			Structure::VectorHashMap<address, uint>											_spotLightColorBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightColorSRBuffer;
			Structure::VectorHashMap<address, ushort>										_spotLightShadowIndexBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightShadowIndexSRBuffer;

			BufferUpdateType																_pointLightBufferUpdateType;
			BufferUpdateType																_spotLightBufferUpdateType;
			BufferUpdateType																_directionalLightBufferUpdateType;

			bool																			_forceUpdateDL;
			bool																			_forceUpdatePL;
			bool																			_forceUpdateSL;

		public:
			LightManager(void);
			~LightManager(void);

		private:
			void UpdateSRBufferUsingMapDiscard(	ID3D11DeviceContext* context,
												const std::function<uint(const Light::LightForm*)>& getShadowIndexInEachShadowLights);
			void UpdateSRBufferUsingMapNoOverWrite(ID3D11DeviceContext* context);

		public:
			void InitializeAllShaderResourceBuffer();
			void DestroyAllShaderReourceBuffer();

			uint Add(Light::LightForm*& light);
			void UpdateSRBuffer(const Device::DirectX* dx,
								const std::function<uint(const Light::LightForm*)>& getShadowIndexInEachShadowLights);
			void ComputeDirectionalLightViewProj(const Intersection::BoundBox& sceneBoundBox, float directionalLightShadowMapResolution);

			bool Has(Light::LightForm*& light) const;
			uint FetchLightIndexInEachLights(const Light::LightForm* light) const;

			void Delete(const Light::LightForm*& light);
			void DeleteAll();
			void Destroy();

			uint GetPackedLightCount() const;

		public:
			GET_ACCESSOR(PointLightTransformSRBuffer,			const Buffer::ShaderResourceBuffer*,	_pointLightTransformSRBuffer);
			GET_ACCESSOR(PointLightColorSRBuffer,				const Buffer::ShaderResourceBuffer*,	_pointLightColorSRBuffer);
			GET_ACCESSOR(PointLightShadowIndexSRBuffer,			const Buffer::ShaderResourceBuffer*,	_pointLightShadowIndexSRBuffer);

			GET_ACCESSOR(DirectionalLightTransformSRBuffer,		const Buffer::ShaderResourceBuffer*,	_directionalLightTransformSRBuffer);
			GET_ACCESSOR(DirectionalLightParamSRBuffer,			const Buffer::ShaderResourceBuffer*,	_directionalLightParamSRBuffer);
			GET_ACCESSOR(DirectionalLightColorSRBuffer,			const Buffer::ShaderResourceBuffer*,	_directionalLightColorSRBuffer);
			GET_ACCESSOR(DirectionalLightShadowIndexSRBuffer,	const Buffer::ShaderResourceBuffer*,	_directionalLightShadowIndexSRBuffer);

			GET_ACCESSOR(SpotLightTransformSRBuffer,			const Buffer::ShaderResourceBuffer*,	_spotLightTransformSRBuffer);
			GET_ACCESSOR(SpotLightParamSRBuffer,				const Buffer::ShaderResourceBuffer*,	_spotLightParamSRBuffer);
			GET_ACCESSOR(SpotLightColorSRBuffer,				const Buffer::ShaderResourceBuffer*,	_spotLightColorSRBuffer);
			GET_ACCESSOR(SpotLightShadowIndexSRBuffer,			const Buffer::ShaderResourceBuffer*,	_spotLightShadowIndexSRBuffer);
		};	
	}
}