#include "PhysicallyBasedMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

PhysicallyBasedMaterial::PhysicallyBasedMaterial(const std::string& name) 
	: Material(name)
{
	// Texture Slot Setting
	std::vector<unsigned int> psTextureIndices;
	psTextureIndices.push_back( (uint)PSTextureSlot::Diffuse );
	psTextureIndices.push_back( (uint)PSTextureSlot::Normal );
	psTextureIndices.push_back( (uint)PSTextureSlot::Specular );
	psTextureIndices.push_back( (uint)PSTextureSlot::Opacity );

	Material::Init(psTextureIndices);
}

PhysicallyBasedMaterial::~PhysicallyBasedMaterial(void)
{
}

void PhysicallyBasedMaterial::UpdateColor(const Color& color)
{
	_mainColor = color;
	_hasAlpha = (_mainColor.a < 1.0f);
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Diffuse, tex );
	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Normal, tex );
}

void PhysicallyBasedMaterial::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Specular, tex );
}

void PhysicallyBasedMaterial::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Opacity, tex );

	_hasAlpha = tex != nullptr;
	_changedAlpha = true;
}