#include "PhysicallyBasedMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;

PhysicallyBasedMaterial::GBufferParam::GBufferParam()
{
	mainColor.r = mainColor.g = mainColor.b = 1.0f;
	metallic_roughness_emission = 0;

	uvTiling.x = uvTiling.y = 1.0f;
	uvOffset.x = uvOffset.y = 0.0f;
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

	SetRoughness(0.6f);

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
		GetMainColor(param.mainColor);

		float metallic = 0.0f, roughness = 0.0f, emission = 0.0f;
		GetMetallic(metallic);
		GetRoughness(roughness);
		GetEmission(emission);		

		uint scaledMetallic		= (uint)(metallic	* 1024);
		uint scaledRoughness	= (uint)(roughness	* 1024);
		uint scaledEmission		= (uint)(emission	* 1024);

		param.metallic_roughness_emission =
			(scaledMetallic << 20 ) | (scaledRoughness << 10 ) | (scaledEmission);

		GetUVTiling(param.uvTiling);
		GetUVOffset(param.uvOffset);

		uint dummy = 0;

		// diffuse tex
		{
			bool hasTexture = FindTexture(dummy, (uint)InputTextureShaderIndex::Diffuse) != nullptr;
			if(hasTexture)
				param.mainColor.r *= -1.0f;
		}
		// normal tex
		{
			bool hasTexture = FindTexture(dummy, (uint)InputTextureShaderIndex::Normal) != nullptr;
			if(hasTexture)
				param.mainColor.g *= -1.0f;
		}
		// specular tex
		{
			bool hasTexture = FindTexture(dummy, (uint)InputTextureShaderIndex::Specular) != nullptr;
			if(hasTexture)
				param.mainColor.b *= -1.0f;
		}

		_gbufferCB->UpdateSubResource(dx->GetContext(), &param);
		_constBufferUpdateCounter = GetVariableUpdateCounter();

		uint idx = (uint)InputConstBufferShaderIndex::Material;
		SetConstBufferUseShaderSlotIndex(idx, _gbufferCB, ShaderForm::Usage(false, false, false, true));
	}
}

void PhysicallyBasedMaterial::UpdateMainColor(const Color& color)
{
	GBufferParam::MainColor mainColor;
	{
		mainColor.r = color.r;
		mainColor.g = color.g;
		mainColor.b = color.b;
	}
	SetVariable("MainColor", mainColor);

	_hasAlpha = (color.a < 1.0f);
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Diffuse, tex, ShaderForm::Usage(false, false, false, true));
	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateNormalMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Normal, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateSpecularMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Specular, tex, ShaderForm::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateOpacityMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Opacity, tex, ShaderForm::Usage(false, false, false, true));

	_hasAlpha = tex != nullptr;
	_changedAlpha = true;
}