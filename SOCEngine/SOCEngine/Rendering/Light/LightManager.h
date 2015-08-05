#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorMap.h"

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		private:
			Structure::VectorMap<std::string, const Light::LightForm*>						_lights;

			Structure::VectorMap<std::string, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Structure::VectorMap<std::string, uint>											_pointLightColorBuffer;

			Structure::VectorMap<std::string, Light::LightForm::LightTransformBuffer>		_directionalLightTransformBuffer;
			Structure::VectorMap<std::string, Light::DirectionalLight::Params>				_directionalLightParammBuffer;
			Structure::VectorMap<std::string, uint>											_directionalLightColorBuffer;

			Structure::VectorMap<std::string, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Structure::VectorMap<std::string, uint>											_spotLightColorBuffer;
			Structure::VectorMap<std::string, Light::SpotLight::Params>						_spotLightParamBuffer;

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
		};
	}
}