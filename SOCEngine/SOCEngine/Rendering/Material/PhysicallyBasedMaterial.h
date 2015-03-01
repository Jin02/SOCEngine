#pragma once

#include "Material.h"

namespace Rendering
{
	class PhysicallyBasedMaterial : public Material
	{
	public:
		enum class PSTextureSlot { Diffuse, Normal, Specular, Opacity };

	public:
		struct GBufferParam
		{
			Color			mainColor; // a is opacity
			float			roughness;
			float			fresnel0;
			Math::Vector2	uvTiling;
		};

	private:
		Color		_mainColor;
		float		_roughness;
		float		_fresnel0;

	public:
		PhysicallyBasedMaterial(const std::string& name);
		~PhysicallyBasedMaterial(void);

	public:
		void UpdateColor(const Color& color);

		void UpdateDiffuseMap(const Rendering::Texture::Texture* tex);
		void UpdateNormalMap(const Rendering::Texture::Texture* tex);
		void UpdateSpecularMap(const Rendering::Texture::Texture* tex);
		void UpdateOpacityMap(const Rendering::Texture::Texture* tex);
	};
}