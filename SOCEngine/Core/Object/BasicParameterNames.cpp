#include "BasicParameterNames.h"

namespace Rendering
{
	namespace Shader
	{
		const char* BasicParameterNames::GetWorldMatrix()
		{
			static const char *worldMat	= "worldMat";
			return worldMat;
		}

		const char* BasicParameterNames::GetViewMatrix()
		{
			static const char *viewMat	= "viewMat";
			return viewMat;
		}

		const char* BasicParameterNames::GetProjMatrix()
		{
			static const char *projMat = "projMat";
			return projMat;
		}

		const char* BasicParameterNames::GetViewProjMatrix()
		{
			static const char *viewProjMat = "viewProjMat";
			return viewProjMat;
		}

		const char* BasicParameterNames::GetWorldViewProjMatrix()
		{
			static const char *worldViewProjMat	= "worldViewProjMat";
			return worldViewProjMat;
		}

		const char* BasicParameterNames::GetWorldViewInvTnsMatrix()
		{
			static const char *wit = "worldViewInvTns";
			return wit;
		}

		const char* BasicParameterNames::GetViewPos()
		{
			static const char *viewPos = "cameraPos";
			return viewPos;
		}

		const char* BasicParameterNames::GetMaterial()
		{
			static const char *material = "material";
			return material;
		}

		const char* BasicParameterNames::GetAmbient()
		{
			static const char *param = "ambient";
			return param;
		}
	
		const char* BasicParameterNames::GetDiffuse()
		{
			static const char *param = "diffuse";
			return param;
		}

		const char* BasicParameterNames::GetSpecular()
		{
			static const char *param = "specular";
			return param;
		}

		const char* BasicParameterNames::GetMaterialEmissive()
		{
			static const char *param = "emissive";
			return param;
		}

		const char* BasicParameterNames::GetMaterialTransparent()
		{
			static const char *param = "transparent";
			return param;
		}

		const char* BasicParameterNames::GetMaterialShininess()
		{
			static const char *shininess = "shininess";
			return shininess;
		}

		const char* BasicParameterNames::GetLight()
		{
			static const char *light = "lights";
			return light;
		}

		const char* BasicParameterNames::GetLightType()
		{
			static const char *param = "type";
			return param;
		}

		const char* BasicParameterNames::GetLightDir()
		{
			static const char *param = "dir";
			return param;
		}

		const char* BasicParameterNames::GetLightPos()
		{
			static const char *param = "pos";
			return param;
		}

		const char* BasicParameterNames::GetLightRange()
		{
			static const char *range = "range";
			return range;
		}

		const char* BasicParameterNames::GetLightSpotAngle()
		{
			static const char *spotAngle = "spotAngle";
			return spotAngle;
		}

		const char* BasicParameterNames::GetLightNum()
		{
			static const char *n = "lightCount";
			return n;
		}

		void BasicParameterNames::GetMaterialElement(std::string& outString, const char* materialComponent)
		{
			std::string str = GetMaterial();
			str += ".";
			str += materialComponent;

			outString = str;
		}
	}
}