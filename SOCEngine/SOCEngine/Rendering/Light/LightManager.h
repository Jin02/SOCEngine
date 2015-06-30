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
			bool HasKey(const std::string& key);

			void Delete(const std::string& key, Light::LightForm::LightType type);
			void DeleteAll();
		};
	}
}