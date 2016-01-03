#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorHashMap.h"
#include "ShaderResourceBuffer.h"

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
		private:
			struct Lights
			{
				Light::LightForm* light;
				uint prevTransformUpdateCounter;
				Lights(Light::LightForm* _light, uint updateCounter) 
					: light(_light), prevTransformUpdateCounter(updateCounter) {}
				~Lights(){}
			};

			Structure::VectorHashMap<address, Lights>	_lights;
			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Structure::VectorHashMap<address, uint>											_pointLightColorBuffer;

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_directionalLightTransformBuffer;
			Structure::VectorHashMap<address, Light::DirectionalLight::Params>				_directionalLightParamBuffer;
			Structure::VectorHashMap<address, uint>											_directionalLightColorBuffer;

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Structure::VectorHashMap<address, Light::SpotLight::Params>						_spotLightParamBuffer;
			Structure::VectorHashMap<address, uint>											_spotLightColorBuffer;

			Structure::VectorHashMap<address, Shadow::PointLightShadow::Param>				_pointLightShadowParamBuffer;
			Structure::VectorHashMap<address, Shadow::SpotLightShadow::Param>				_spotLightShadowParamBuffer;
			Structure::VectorHashMap<address, Shadow::DirectionalLightShadow::Param>		_directionalLightShadowParamBuffer;

			Structure::VectorHashMap<address, uint>											_pointLightShadowColorBuffer;
			Structure::VectorHashMap<address, uint>											_spotLightShadowColorBuffer;
			Structure::VectorHashMap<address, uint>											_directionalLightShadowColorBuffer;

			Buffer::ShaderResourceBuffer*	_pointLightTransformSRBuffer;
			Buffer::ShaderResourceBuffer*	_pointLightColorSRBuffer;

			Buffer::ShaderResourceBuffer*	_directionalLightTransformSRBuffer;
			Buffer::ShaderResourceBuffer*	_directionalLightParamSRBuffer;
			Buffer::ShaderResourceBuffer*	_directionalLightColorSRBuffer;

			Buffer::ShaderResourceBuffer*	_spotLightTransformSRBuffer;
			Buffer::ShaderResourceBuffer*	_spotLightParamSRBuffer;
			Buffer::ShaderResourceBuffer*	_spotLightColorSRBuffer;

			Buffer::ShaderResourceBuffer*	_directionalLightShadowParamSRBuffer;
			Buffer::ShaderResourceBuffer*	_spotLightShadowParamSRBuffer;
			Buffer::ShaderResourceBuffer*	_pointLightShadowParamSRBuffer;

			Buffer::ShaderResourceBuffer*	_directionalLightShadowColorSRBuffer;
			Buffer::ShaderResourceBuffer*	_spotLightShadowColorSRBuffer;
			Buffer::ShaderResourceBuffer*	_pointLightShadowColorSRBuffer;

			enum class BufferUpdateType : uint
			{
				Overall,
				Selective
			};

			BufferUpdateType _pointLightBufferUpdateType;
			BufferUpdateType _spotLightBufferUpdateType;
			BufferUpdateType _directionalLightBufferUpdateType;

		public:
			LightManager(void);
			~LightManager(void);

		private:
			void UpdateBufferUsingMapDiscard(ID3D11DeviceContext* context, bool useVSM);
			void UpdateBufferUsingMapNoOverWrite(ID3D11DeviceContext* context, bool useVSM);

		public:
			void InitializeAllShaderResourceBuffer();
			void DestroyAllShaderReourceBuffer();

			void Add(Light::LightForm*& light);
			void UpdateBuffer(const Device::DirectX* dx, bool useVSM);
			void ComputeAllLightViewProj(const Intersection::BoundBox& sceneBoundBox);

			bool Has(Light::LightForm*& light) const;
			uint GetLightIndexInEachLights(const Light::LightForm*& light) const;

			void Delete(const Light::LightForm*& light);
			void DeleteAll();

			uint GetPackedLightCount() const;

		public:
			GET_ACCESSOR(PointLightTransformSRBuffer,			const Buffer::ShaderResourceBuffer*, _pointLightTransformSRBuffer);
			GET_ACCESSOR(PointLightColorSRBuffer,				const Buffer::ShaderResourceBuffer*, _pointLightColorSRBuffer);

			GET_ACCESSOR(DirectionalLightTransformSRBuffer,		const Buffer::ShaderResourceBuffer*, _directionalLightTransformSRBuffer);
			GET_ACCESSOR(DirectionalLightParamSRBuffer,			const Buffer::ShaderResourceBuffer*, _directionalLightParamSRBuffer);
			GET_ACCESSOR(DirectionalLightColorSRBuffer,			const Buffer::ShaderResourceBuffer*, _directionalLightColorSRBuffer);

			GET_ACCESSOR(SpotLightTransformSRBuffer,			const Buffer::ShaderResourceBuffer*, _spotLightTransformSRBuffer);
			GET_ACCESSOR(SpotLightParamSRBuffer,				const Buffer::ShaderResourceBuffer*, _spotLightParamSRBuffer);
			GET_ACCESSOR(SpotLightColorSRBuffer,				const Buffer::ShaderResourceBuffer*, _spotLightColorSRBuffer);

			GET_ACCESSOR(DirectionalLightShadowParamSRBuffer,	const Buffer::ShaderResourceBuffer*, _directionalLightShadowParamSRBuffer);
			GET_ACCESSOR(PointLightShadowParamSRBuffer,			const Buffer::ShaderResourceBuffer*, _pointLightShadowParamSRBuffer);
			GET_ACCESSOR(SpotLightShadowParamSRBuffer,			const Buffer::ShaderResourceBuffer*, _spotLightShadowParamSRBuffer);

			GET_ACCESSOR(DirectionalLightShadowColorSRBuffer,	const Buffer::ShaderResourceBuffer*, _directionalLightShadowColorSRBuffer);
			GET_ACCESSOR(PointLightShadowColorSRBuffer,			const Buffer::ShaderResourceBuffer*, _pointLightShadowColorSRBuffer);
			GET_ACCESSOR(SpotLightShadowColorSRBuffer,			const Buffer::ShaderResourceBuffer*, _spotLightShadowColorSRBuffer);
		};	
	}
}