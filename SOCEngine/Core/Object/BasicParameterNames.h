#pragma once

#include <string>

namespace Rendering
{
	namespace Shader
	{
		enum REQUIRED_TRANSFORM
		{
			WORLD			= 1,
			VIEW			= 2,
			PROJECTION		= 4,
			VIEW_PROJECTION = 8,	//View * Projection
			WORLD_VIEW_PROJECTION = 16,	//World * View * Projection
			WORLD_VIEW_INV_TRANS = 32
		};

		enum REQUIRED_LIGHTING
		{
			MATERIAL			= 1,
			MATERIAL_SHININESS	= 2,

			LIGHT				= 4,
			LIGHT_RANGE			= 8,
			LIGHT_SPOTANGLE		= 16
		};

		class BasicParameterNames
		{
		public:
			static const char *GetWorldMatrix();
			static const char *GetViewMatrix();
			static const char *GetProjMatrix();
			static const char *GetViewProjMatrix();
			static const char *GetWorldViewProjMatrix();
			static const char *GetWorldViewInvTnsMatrix();

			static const char *GetMaterial();
			static const char *GetMaterialEmissive();
			static const char *GetMaterialTransparent();
			static const char *GetMaterialShininess();

			static const char *GetAmbient();
			static const char *GetDiffuse();
			static const char *GetSpecular();

			static const char *GetLight();
			static const char *GetLightType();
			static const char *GetLightDir();
			static const char *GetLightPos();

			static const char *GetLightNum();
			static const char *GetViewPos();

			static const char *GetLightRange();
			static const char *GetLightSpotAngle();

		public:
			static std::string GetMaterialElement(const char* materialComponent)
			{
				std::string str = GetMaterial();
				str += ".";
				str += materialComponent;

				return str;
			}
		};
	}
}