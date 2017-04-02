#pragma once

#include "Material.h"

#define SET_MATERIAL_ACCESSOR(name, type, variable)		inline void Set##name(type t)	{ variable = t; _dirty = true; }
#define UPDATE_TEXTURE(name, bind)\
 inline void Update##name(const Texture::Texture2D& tex) {UpdateMap(Get##name##Key(), bind, tex);}

namespace Rendering
{
	class PhysicallyBasedMaterial : public Material
	{
	public:
		struct Param
		{
			uint mainColor_alpha				= 0;
			uint emissiveColor_Metallic			= 0;
			uint roughness_specularity_existTextureFlag	= 0;

			//상위 2비트는 아직 사용하지 않음
			uint flag_ior					= 0;
			
			static constexpr const char* GetKey() { return "PBRParam"; }
		};

	public:
		using Material::Material;

		void Initialize(Device::DirectX& dx);
		void Destroy();
		void UpdateConstBuffer(Device::DirectX& dx);

		void UpdateMap(const std::string& key, TextureBindIndex bind, const Texture::Texture2D& tex);

		static constexpr const char* GetDiffuseMapKey() { return "DiffuseMap"; }
		static constexpr const char* GetNormalMapKey() { return "NormalMap"; }
		static constexpr const char* GetOpacityMapKey() { return "OpacityMap"; }
		static constexpr const char* GetHeightMapKey() { return "HeightMap"; }
		static constexpr const char* GetMetallicMapKey() { return "MetallicMap"; }
		static constexpr const char* GetOcclusionMapKey() { return "OcclusionMap"; }
		static constexpr const char* GetRoughnessMapKey() { return "RoughnessMap"; }
		static constexpr const char* GetEmissionMapKey() { return "EmissionMap"; }

		UPDATE_TEXTURE(DiffuseMap,		TextureBindIndex::DiffuseMap);
		UPDATE_TEXTURE(NormalMap,		TextureBindIndex::NormalMap);
		UPDATE_TEXTURE(OpacityMap,		TextureBindIndex::OpacityMap);
		UPDATE_TEXTURE(HeightMap,		TextureBindIndex::HeightMap);
		UPDATE_TEXTURE(MetallicMap,		TextureBindIndex::MetallicMap);
		UPDATE_TEXTURE(OcclusionMap,	TextureBindIndex::OcclusionMap);
		UPDATE_TEXTURE(RoughnessMap,	TextureBindIndex::RoughnessMap);
		UPDATE_TEXTURE(EmissionMap,		TextureBindIndex::EmissionMap);

		GET_CONST_ACCESSOR(MainColor,				const Color&,	_mainColor);
		GET_CONST_ACCESSOR(EmissiveColor,			const Color&,	_emissiveColor);
		GET_CONST_ACCESSOR(Metallic,				float,			_metallic);
		GET_CONST_ACCESSOR(Specularity,				float,			_specularity);
		GET_CONST_ACCESSOR(Roughness,				float,			_roughness);
		GET_CONST_ACCESSOR(Flag,					uchar,			_flag);
		GET_CONST_ACCESSOR(IndexOfRefraction,		float,			_ior);
		SET_MATERIAL_ACCESSOR(MainColor,			const Color&,	_mainColor);
		SET_MATERIAL_ACCESSOR(EmissiveColor,		const Color&,	_emissiveColor);
		SET_MATERIAL_ACCESSOR(Metallic,				float,			_metallic);
		SET_MATERIAL_ACCESSOR(Specularity,			float,			_specularity);
		SET_MATERIAL_ACCESSOR(Roughness,			float,			_roughness);
		SET_MATERIAL_ACCESSOR(Flag,					uchar,			_flag);
		SET_MATERIAL_ACCESSOR(IndexOfRefraction,	float,			_ior);

	private:
		bool		_dirty			= true;
		Color		_mainColor		= Color(1.0f, 1.0f, 1.0f, 1.0f);
		Color		_emissiveColor	= Color(0.0f, 0.0f, 0.0f, 0.0f);;
		float		_metallic		= 0.0f;
		float		_specularity	= 0.0f;
		float		_roughness		= 0.0f;
		uchar		_flag			= 0;
		float		_ior			= 0.0f;
	};
}

#undef UPDATE_TEXTURE
