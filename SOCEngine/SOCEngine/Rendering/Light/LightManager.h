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
			Structure::VectorHashMap<std::string, Lights>	_lights;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Structure::VectorHashMap<std::string, uint>											_pointLightColorBuffer;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_directionalLightTransformBuffer;
			Structure::VectorHashMap<std::string, Light::DirectionalLight::Params>				_directionalLightParamBuffer;
			Structure::VectorHashMap<std::string, uint>											_directionalLightColorBuffer;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Structure::VectorHashMap<std::string, Light::SpotLight::Params>						_spotLightParamBuffer;
			Structure::VectorHashMap<std::string, uint>											_spotLightColorBuffer;

			Structure::VectorHashMap<std::string, Shadow::PointLightShadow::Param>				_pointLightShadowParamBuffer;
			Structure::VectorHashMap<std::string, Shadow::SpotLightShadow::Param>				_spotLightShadowParamBuffer;
			Structure::VectorHashMap<std::string, Shadow::DirectionalLightShadow::Param>		_directionalLightShadowParamBuffer;

			Structure::VectorHashMap<std::string, uint>											_pointLightShadowColorBuffer;
			Structure::VectorHashMap<std::string, uint>											_spotLightShadowColorBuffer;
			Structure::VectorHashMap<std::string, uint>											_directionalLightShadowColorBuffer;

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
			void UpdateBufferUsingMapDiscard(ID3D11DeviceContext* context);
			void UpdateBufferUsingMapNoOverWrite(ID3D11DeviceContext* context);

		public:
			void InitializeAllShaderResourceBuffer();
			void DestroyAllShaderReourceBuffer();

			void Add(Light::LightForm* light, const char* key = nullptr);
			void UpdateBuffer(const Device::DirectX* dx);
			void ComputeAllLightViewProj(const Intersection::BoundBox& sceneBoundBox);

			bool HasKey(const std::string& key);

			void Delete(const std::string& key);
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