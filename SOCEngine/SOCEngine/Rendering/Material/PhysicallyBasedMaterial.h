#pragma once

#include "Material.h"

namespace Rendering
{
	class PhysicallyBasedMaterial : public Material
	{
	public:
		struct GBufferParam
		{
			uint			mainColor_alpha;
			uint			emissiveColor_Metallic;
			uint			roughness_specularity_existTextureFlag;
			uint			flag;

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
		void SetMainColor(const Color& color);

		void UpdateDiffuseMap(const Rendering::Texture::Texture2D* tex);
		void UpdateNormalMap(const Rendering::Texture::Texture2D* tex);
		void UpdateOpacityMap(const Rendering::Texture::Texture2D* tex);
		void UpdateHeightMap(const Rendering::Texture::Texture2D* tex);
		void UpdateMetallicMap(const Rendering::Texture::Texture2D* tex);
		void UpdateOcclusionMap(const Rendering::Texture::Texture2D* tex);
		void UpdateRoughnessMap(const Rendering::Texture::Texture2D* tex);
		void UpdateEmissionMap(const Rendering::Texture::Texture2D* tex);

	public:
		GET_ACCESSOR_MATERIAL(MainColor,			Color);
		GET_SET_ACCESSOR_MATERIAL(EmissiveColor,	Color);
		GET_SET_ACCESSOR_MATERIAL(Metallic,			float);
		GET_SET_ACCESSOR_MATERIAL(Specularity,		float);
		GET_SET_ACCESSOR_MATERIAL(Roughness,		float);
		GET_SET_ACCESSOR_MATERIAL(Flag,				uint);
		GET_SET_ACCESSOR_MATERIAL(UVTiling0,		Math::Vector2);
		GET_SET_ACCESSOR_MATERIAL(UVOffset0,		Math::Vector2);
		GET_SET_ACCESSOR_MATERIAL(UVTiling1,		Math::Vector2);
		GET_SET_ACCESSOR_MATERIAL(UVOffset1,		Math::Vector2);
	};
}