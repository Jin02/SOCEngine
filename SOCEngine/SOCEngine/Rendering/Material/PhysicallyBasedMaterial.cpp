#include "PhysicallyBasedMaterial.h"
#include "Utility.h"
#include "Director.h"
#include "BindIndexInfo.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;

PhysicallyBasedMaterial::GBufferParam::GBufferParam()
{
	mainColor_alpha					= 0;
	emissiveColor_Metallic				= 0;
	roughness_specularity_existTextureFlag		= 0;
	flag_ior					= 0;

	uvTiling0.x = uvTiling0.y = uvTiling1.x = uvTiling1.y = 1.0f;
	uvOffset0.x = uvOffset0.y = uvOffset1.x = uvOffset1.y = 0.0f;
}

PhysicallyBasedMaterial::GBufferParam::~GBufferParam(){}

PhysicallyBasedMaterial::PhysicallyBasedMaterial(const std::string& name)
	: Material(name, Type::PhysicallyBasedModel), 
	_gbufferCB(nullptr), _constBufferUpdateCounter(0)
{
}

PhysicallyBasedMaterial::~PhysicallyBasedMaterial(void)
{
	Destroy();
}

void PhysicallyBasedMaterial::Initialize()
{
	ASSERT_COND_MSG(_gbufferCB == nullptr, "Error, gbuffer const buffer was already allocated");

	SetMainColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	SetEmissiveColor(Color(0.0f, 0.0f, 0.0f, 0.0f));

	SetRoughness(0.5f);
	SetMetallic(0.5f);
	SetSpecularity(0.5f);

	_gbufferCB = new ConstBuffer;
	_gbufferCB->Initialize(sizeof(GBufferParam));
}

void PhysicallyBasedMaterial::Destroy()
{
	SAFE_DELETE(_gbufferCB);
}

void PhysicallyBasedMaterial::UpdateConstBuffer(const Device::DirectX* dx)
{
	if( _constBufferUpdateCounter < GetVariableUpdateCounter() )
	{
		GBufferParam param;

		// mainColor_alpha
		{
			Color mainColor;
			GetMainColor(mainColor);

			param.mainColor_alpha =	((uint(mainColor.r * 255.0f) & 0xff) << 24	|
									 (uint(mainColor.g * 255.0f) & 0xff) << 16	|
									 (uint(mainColor.b * 255.0f) & 0xff) << 8	|
									 (uint(mainColor.a * 255.0f) & 0xff) );
		}

		// emissvieColor_metallic
		{
			Color emissiveColor;
			GetEmissiveColor(emissiveColor);
	
			float metallic = 0.0f;
			GetMetallic(metallic);

			param.emissiveColor_Metallic = ((uint(emissiveColor.r * 255.0f) & 0xff) << 24	|
											(uint(emissiveColor.g * 255.0f) & 0xff) << 16	|
											(uint(emissiveColor.b * 255.0f) & 0xff) << 8	|
											(uint(metallic * 255.0f) & 0xff) );
		}

		// Roughness, Flag, ExistTextureFlag
		{
			float specularity = 0.0f;
			GetSpecularity(specularity);
			uint scaledSpecularity = uint(specularity * 255.0f) & 0xff;

			float roughness = 0.0f;
			GetRoughness(roughness);
			uint scaledRoughness = uint(roughness * 255.0f) & 0xff;

			// Exist Texture Flag
			uint existTextureFlag = 0;
			{
				auto GetExistTextureFlag = [](PhysicallyBasedMaterial* mat, TextureBindIndex bind, uint flagPos)
				{
					uint dummy = 0;
					bool hasTexture = mat->FindTexture(dummy, uint(bind)) != nullptr;	

					return uint(hasTexture) << flagPos;
				};

				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::DiffuseMap,		0);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::NormalMap,		1);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::OpacityMap,		2);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::HeightMap,		3);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::MetallicMap,	4);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::OcclusionMap,	5);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::RoughnessMap,	6);
				existTextureFlag |= GetExistTextureFlag(this, TextureBindIndex::EmissionMap,	7);
			}

			uint resultFlag = (scaledRoughness << 24) | (scaledSpecularity << 16) | (existTextureFlag & 0xffff);
			param.roughness_specularity_existTextureFlag = resultFlag;
		}

		unsigned char flag = 0;
		GetFlag(flag);
		
		float ior = 0.0f;
		GetIndexOfRefraction(ior);
		ior = min(max(0.0f, ior), 1.0f) * 255.0f;
				
		param.flag_ior = (static_cast<uint>(flag) << 8) | static_cast<uint>(ior);

		GetUVTiling0(param.uvTiling0);
		GetUVOffset0(param.uvOffset0);
		GetUVTiling1(param.uvTiling1);
		GetUVOffset1(param.uvOffset1);

		_gbufferCB->UpdateSubResource(dx->GetContext(), &param);
		_constBufferUpdateCounter = GetVariableUpdateCounter();

		uint idx = (uint)ConstBufferBindIndex::PhysicallyBasedMaterial;
		SetConstBufferUseBindIndex(idx, _gbufferCB, ShaderForm::Usage(false, false, false, true));
	}
}

void PhysicallyBasedMaterial::SetMainColor(const Color& color)
{
	SetVariable("MainColor", color);

	_hasAlpha = (color.a < 1.0f);
	_alpha = color.a;
}

void PhysicallyBasedMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::DiffuseMap, tex, ShaderForm::Usage(false, false, false, true));
	_hasAlpha = tex->GetHasAlpha();
}

void PhysicallyBasedMaterial::UpdateNormalMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::NormalMap, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateOpacityMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::OpacityMap, tex, ShaderForm::Usage(false, false, false, true));
	_hasAlpha = tex != nullptr;
}

void PhysicallyBasedMaterial::UpdateHeightMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::HeightMap, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateMetallicMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::MetallicMap, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateOcclusionMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::OcclusionMap, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateRoughnessMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::RoughnessMap, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateEmissionMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseBindIndex( (uint)TextureBindIndex::EmissionMap, tex, ShaderForm::Usage(false, false, false, true));
}
