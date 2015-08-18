#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorMap.h"
#include "VectorHashMap.h"

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		private:
			struct Lights
			{
				const Light::LightForm* light;
				uint prevTransformUpdateCounter;
				Lights(const Light::LightForm* _light, uint updateCounter) 
					: light(_light), prevTransformUpdateCounter(updateCounter) {}
				~Lights(){}
			};
			Structure::VectorHashMap<std::string, Lights>	_lights;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Structure::VectorHashMap<std::string, uint>											_pointLightColorBuffer;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_directionalLightTransformBuffer;
			Structure::VectorHashMap<std::string, Light::DirectionalLight::Params>				_directionalLightParammBuffer;
			Structure::VectorHashMap<std::string, uint>											_directionalLightColorBuffer;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Structure::VectorHashMap<std::string, uint>											_spotLightColorBuffer;
			Structure::VectorHashMap<std::string, Light::SpotLight::Params>						_spotLightParamBuffer;

			uint	_directionalLightUpdateCounter;
			uint	_spotLightUpdateCounter;
			uint	_pointLightUpdateCounter;

		public:
			LightManager(void);
			~LightManager(void);

		public:
			void Add(const Light::LightForm* light, const char* key = nullptr);
			void UpdateBuffer();

			bool HasKey(const std::string& key);

			void Delete(const std::string& key);
			void DeleteAll();

		public:
			GET_ACCESSOR(DirectionalLightTransformBuffer,	const Light::LightForm::LightTransformBuffer*,	_directionalLightTransformBuffer.GetVector().data());
			GET_ACCESSOR(DirectionalLightParamBuffer,		const Light::DirectionalLight::Params*,			_directionalLightParammBuffer.GetVector().data());
			GET_ACCESSOR(DirectionalLightColorBuffer,		const uint*,									_directionalLightColorBuffer.GetVector().data());

			GET_ACCESSOR(SpotLightTransformBuffer,			const Light::LightForm::LightTransformBuffer*,	_spotLightTransformBuffer.GetVector().data());
			GET_ACCESSOR(SpotLightParamBuffer,				const Light::SpotLight::Params*,				_spotLightParamBuffer.GetVector().data());
			GET_ACCESSOR(SpotLightColorBuffer,				const uint*,									_spotLightColorBuffer.GetVector().data());

			GET_ACCESSOR(PointLightTransformBuffer,			const Light::LightForm::LightTransformBuffer*,	_pointLightTransformBuffer.GetVector().data());
			GET_ACCESSOR(PointLightColorBuffer,				const uint*,									_pointLightColorBuffer.GetVector().data());

			GET_ACCESSOR(DirectionalLightUpdateCounter, uint,	_directionalLightUpdateCounter);
			GET_ACCESSOR(SpotLightUpdateCounter,		uint,	_spotLightUpdateCounter);
			GET_ACCESSOR(PointLightUpdateCounter,		uint,	_pointLightUpdateCounter);
		};
	}
}