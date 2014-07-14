#pragma once

#include "EngineMath.h"

namespace Rendering
{
	namespace Light
	{
		struct LightParameters
		{
			Math::Vector3 ambient;
			Math::Vector3 diffuse;
			Math::Vector3 specular;

			float range;

			Math::Vector4 lightPos;
			Math::Vector3 lightDir;

			float spotAngle;
			int type;

		public:
			LightParameters();
			~LightParameters()
			{
			}

			void SetData(Math::Vector3 &ambient, Math::Vector3 &diffuse, Math::Vector3 &specular, float range,
				Math::Vector3 &lightPos, Math::Vector3& lightdir, float spotAngle, int type)
			{
				this->ambient = ambient;
				this->diffuse = diffuse;
				this->specular = specular;
				this->range = range;
				this->lightPos = Math::Vector4(lightPos.x, lightPos.y, lightPos.z, 1.0f);
				this->lightDir = lightdir;
				this->spotAngle = spotAngle;
				this->type = type;
			}
		};

	}
}