#pragma once

#include "Color.h"
#include <vector>
#include <string>

namespace Rendering
{
	namespace Material
	{
		//ambient, diffuse, specular 모두 factor 연산은 해둠.
		struct MaterialElements
		{
			Color ambient;
			Color diffuse;
			Color specular;
			Color emissive;

			float transparentFactor;
			float shininess;

			MaterialElements()
			{
				shininess = 20.0f;
				transparentFactor = 1.0f;

				ambient = Color::gray();
				diffuse = Color::white();
				specular = Color::white();
				emissive = Color::black();
			}

			MaterialElements(Color &ambient, Color &diffuse, Color &specular, float shininess, float transparentFactor = 1.0f)
			{
				this->ambient = ambient;
				this->diffuse = diffuse;
				this->specular = specular;
				this->emissive = Rendering::Color::black();

				this->transparentFactor = transparentFactor;
				this->shininess = shininess;
			}

			MaterialElements(Color &ambient, Color &diffuse, Color &specular, Color &emissive, float shininess, float transparentFactor = 1.0f)
			{
				this->ambient = ambient;
				this->diffuse = diffuse;
				this->specular = specular;
				this->emissive = emissive;

				this->transparentFactor = transparentFactor;
				this->shininess = shininess;
			}

			MaterialElements* Clone()
			{
				return new MaterialElements(ambient, diffuse, specular, emissive, transparentFactor, shininess);
			}
		};
	}

	struct MeshTextureNames
	{
		std::string diffuse;
		std::string emissive;
		std::string specular;
		std::string specularFactor;
		std::string bump;
		std::string bumpFactor;
		std::string transparent;
	};
}