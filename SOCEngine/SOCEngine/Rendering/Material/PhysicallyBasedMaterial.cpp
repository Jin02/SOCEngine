#include "PhysicallyBasedMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

PhysicallyBasedMaterial::PhysicallyBasedMaterial(const std::string& name)
	: Material(name, Type::PhysicallyBasedModel), 
	_gbufferCB(nullptr), _constBufferUpdateCounter(0)
{
}

PhysicallyBasedMaterial::~PhysicallyBasedMaterial(void)
{
	SAFE_DELETE(_gbufferCB);
}

void PhysicallyBasedMaterial::UpdateConstBuffer(Device::DirectX* dx)
{
	if(_gbufferCB == nullptr)
	{
		_gbufferCB = new ConstBuffer;
		_gbufferCB->Initialize(sizeof(GBufferParam));
	}

	if( _constBufferUpdateCounter < GetVariableUpdateCounter() )
	{
		GBufferParam param;
		GetMainColor(param.mainColor);
		GetRoughness(param.roughness);
		GetFresnel_0(param.fresnel0);
		GetMetallic(param.metallic);
		GetUVTiling(param.uvTiling);

		_gbufferCB->UpdateSubResource(dx->GetContext(), &param);
		_constBufferUpdateCounter = GetVariableUpdateCounter();

		UpdateConstBuffer_ShaderSlotIndex(GBufferShaderSlotIndex, _gbufferCB);
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
	UpdateTexture_ShaderSlotIndex( (uint)PSTextureSlot::Diffuse, tex );
	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateNormalMap(const Rendering::Texture::Texture2D* tex)
{
	UpdateTexture_ShaderSlotIndex( (uint)PSTextureSlot::Normal, tex );
}

void PhysicallyBasedMaterial::UpdateSpecularMap(const Rendering::Texture::Texture2D* tex)
{
	UpdateTexture_ShaderSlotIndex( (uint)PSTextureSlot::Specular, tex );
}

void PhysicallyBasedMaterial::UpdateOpacityMap(const Rendering::Texture::Texture2D* tex)
{
	UpdateTexture_ShaderSlotIndex( (uint)PSTextureSlot::Opacity, tex );

	_hasAlpha = tex != nullptr;
	_changedAlpha = true;
}