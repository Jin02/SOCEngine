#pragma once

#include "Material.h"

namespace Rendering
{
	class PhysicallyBasedMaterial : public Material
	{
	public:
		enum class InputTextureShaderIndex : uint
		{ 
			Diffuse		= 0,
			Normal		= 1,
			Specular	= 2,
			Opacity		= 3
		};

		enum class InputConstBufferShaderIndex : uint
		{
			Transform	= 1,
			Material	= 2
		};

	public:
		struct GBufferParam
		{
			struct MainColor { float r, g, b; };
			MainColor		mainColor;
			float			metallic;
			float			roughness;
			float			fresnel0;
			Math::Vector2	uvTiling;

			GBufferParam();
			~GBufferParam();
		};

	private:
		Buffer::ConstBuffer*	_gbufferCB;

	private:
		uint					_constBufferUpdateCounter;

	public:
		PhysicallyBasedMaterial(const std::string& name);
		~PhysicallyBasedMaterial(void);

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
		GET_SET_ACCESSOR_MATERIAL(Metallic,		float);
		GET_SET_ACCESSOR_MATERIAL(Roughness,	float);
		GET_SET_ACCESSOR_MATERIAL(Fresnel_0,	float);
		GET_SET_ACCESSOR_MATERIAL(UVTiling,		Math::Vector2);
	};
}