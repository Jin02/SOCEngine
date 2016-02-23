#pragma once

#include "Material.h"

namespace Rendering
{
	class PhysicallyBasedMaterial : public Material
	{
	public:
		struct GBufferParam
		{
			struct MainColor { float r, g, b; };
			MainColor		mainColor;
			uint			alpha_metallic_roughness_emission;

			Color			emissionColor;;

			Math::Vector2	uvTiling0;
			Math::Vector2	uvOffset0;

			Math::Vector2	uvTiling1;
			Math::Vector2	uvOffset1;

			GBufferParam();
			~GBufferParam();
		};

	private:
		Buffer::ConstBuffer*	_gbufferCB;
		uint					_constBufferUpdateCounter;

	public:
		PhysicallyBasedMaterial(const std::string& name);
		virtual ~PhysicallyBasedMaterial(void);

	public:
		virtual void Initialize();
		virtual void Destroy();
		virtual void UpdateConstBuffer(const Device::DirectX* dx);

	public:
		void UpdateMainColor(const Color& color);

		void UpdateDiffuseMap(const Rendering::Texture::Texture2D* tex);
		void UpdateNormalMap(const Rendering::Texture::Texture2D* tex);
		void UpdateSpecularMap(const Rendering::Texture::Texture2D* tex);
		void UpdateOpacityMap(const Rendering::Texture::Texture2D* tex);

	public:
		GET_ACCESSOR_MATERIAL(MainColor,		GBufferParam::MainColor);

		GET_SET_ACCESSOR_MATERIAL(EmissionColor,Color);
		GET_SET_ACCESSOR_MATERIAL(Metallic,		float);
		GET_SET_ACCESSOR_MATERIAL(Roughness,	float);
		GET_SET_ACCESSOR_MATERIAL(Emission,		float);
		GET_SET_ACCESSOR_MATERIAL(UVTiling0,	Math::Vector2);
		GET_SET_ACCESSOR_MATERIAL(UVOffset0,	Math::Vector2);
		GET_SET_ACCESSOR_MATERIAL(UVTiling1,	Math::Vector2);
		GET_SET_ACCESSOR_MATERIAL(UVOffset1,	Math::Vector2);
	};
}