#pragma once

#include "MaterialForm.hpp"

#define UPDATE_TEXTURE(name, bind)\
 inline void Regist##name(const Texture::Texture2D& tex) {RegistTexture(Get##name##Key(), bind, tex);}

#define GET_CONST_TEXTURE(name)\
 inline const BindTextured2D* Get##name() const { return GetTextures().Find(Get##name##Key()); }

#define HAS_TEXTURE(name)\
 inline bool Has##name() const { return GetTextures().Has(Get##name##Key()); }

namespace Rendering
{
	namespace Material
	{
		class PhysicallyBasedMaterial : public MaterialForm
		{
		public:
			struct ParamCB
			{
				uint mainColor_alpha						= 0;
				uint emissiveColor_Metallic					= 0;
				uint roughness_specularity_existTextureFlag	= 0;

				// TODO: 상위 2비트는 아직 사용하지 않음
				uint flag_ior = 0;

				static constexpr const char* GetKey() { return "PBRParam"; }
			};

		public:
			using MaterialForm::MaterialForm;

			void Initialize(Device::DirectX& dx);
			void Destroy();
			void UpdateConstBuffer(Device::DirectX& dx);

			void RegistTexture(const std::string& key, TextureBindIndex bind, const Texture::Texture2D& tex);

			static constexpr const char* GetDiffuseMapKey()		{ return "DiffuseMap";		}
			static constexpr const char* GetNormalMapKey()		{ return "NormalMap";		}
			static constexpr const char* GetOpacityMapKey()		{ return "OpacityMap";		}
			static constexpr const char* GetHeightMapKey()		{ return "HeightMap";		}
			static constexpr const char* GetMetallicMapKey()	{ return "MetallicMap";		}
			static constexpr const char* GetOcclusionMapKey()	{ return "OcclusionMap";	}
			static constexpr const char* GetRoughnessMapKey()	{ return "RoughnessMap";	}
			static constexpr const char* GetEmissionMapKey()	{ return "EmissionMap";		}
			static constexpr const char* GetParamCBKey()		{ return ParamCB::GetKey();	}

			UPDATE_TEXTURE(DiffuseMap,		TextureBindIndex::DiffuseMap);
			UPDATE_TEXTURE(NormalMap,		TextureBindIndex::NormalMap);
			UPDATE_TEXTURE(OpacityMap,		TextureBindIndex::OpacityMap);
			UPDATE_TEXTURE(HeightMap,		TextureBindIndex::HeightMap);
			UPDATE_TEXTURE(MetallicMap,		TextureBindIndex::MetallicMap);
			UPDATE_TEXTURE(OcclusionMap,	TextureBindIndex::OcclusionMap);
			UPDATE_TEXTURE(RoughnessMap,	TextureBindIndex::RoughnessMap);
			UPDATE_TEXTURE(EmissionMap,		TextureBindIndex::EmissionMap);

			GET_CONST_TEXTURE(DiffuseMap);
			GET_CONST_TEXTURE(NormalMap);
			GET_CONST_TEXTURE(OpacityMap);
			GET_CONST_TEXTURE(HeightMap);
			GET_CONST_TEXTURE(MetallicMap);
			GET_CONST_TEXTURE(OcclusionMap);
			GET_CONST_TEXTURE(RoughnessMap);
			GET_CONST_TEXTURE(EmissionMap);

			HAS_TEXTURE(DiffuseMap);
			HAS_TEXTURE(NormalMap);
			HAS_TEXTURE(OpacityMap);
			HAS_TEXTURE(HeightMap);
			HAS_TEXTURE(MetallicMap);
			HAS_TEXTURE(OcclusionMap);
			HAS_TEXTURE(RoughnessMap);
			HAS_TEXTURE(EmissionMap);

			inline const Buffer::ConstBuffer& GetParamCB() const
			{
				return GetConstBuffers().Find(GetParamCBKey())->resource;
			}

			GET_CONST_ACCESSOR(MainColor,			const Color&,	_mainColor);
			GET_CONST_ACCESSOR(EmissiveColor,		const Color&,	_emissiveColor);
			GET_CONST_ACCESSOR(Metallic,			float,			_metallic);
			GET_CONST_ACCESSOR(Specularity,			float,			_specularity);
			GET_CONST_ACCESSOR(Roughness,			float,			_roughness);
			GET_CONST_ACCESSOR(Flag,				uchar,			_flag);
			GET_CONST_ACCESSOR(IndexOfRefraction,	float,			_ior);
			SET_ACCESSOR_DIRTY(MainColor,			const Color&,	_mainColor);
			SET_ACCESSOR_DIRTY(EmissiveColor,		const Color&,	_emissiveColor);
			SET_ACCESSOR_DIRTY(Metallic,			float,			_metallic);
			SET_ACCESSOR_DIRTY(Specularity,			float,			_specularity);
			SET_ACCESSOR_DIRTY(Roughness,			float,			_roughness);
			SET_ACCESSOR_DIRTY(Flag,				uchar,			_flag);
			SET_ACCESSOR_DIRTY(IndexOfRefraction,	float,			_ior);

			GET_CONST_ACCESSOR(Dirty,				bool,			_dirty);

		private:
			Color		_mainColor		= Color(1.0f, 1.0f, 1.0f, 1.0f);
			Color		_emissiveColor	= Color(0.0f, 0.0f, 0.0f, 0.0f);;
			float		_metallic		= 0.0f;
			float		_specularity	= 0.0f;
			float		_roughness		= 0.0f;
			float		_ior			= 0.0f;
			uchar		_flag			= 0;
			bool		_dirty			= true;
		};
	}
}

#undef UPDATE_TEXTURE
