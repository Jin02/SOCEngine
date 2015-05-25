#include "PhysicallyBasedMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

PhysicallyBasedMaterial::PhysicallyBasedMaterial(Material* material)
	: _baseMaterial(material)
{
}

PhysicallyBasedMaterial::~PhysicallyBasedMaterial(void)
{
}

void PhysicallyBasedMaterial::UpdateColor(const Color& color)
{
	_baseMaterial->SetVariable("mainColor", color);

	_baseMaterial->_hasAlpha = (color.a < 1.0f);
	_baseMaterial->_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	_baseMaterial->UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Diffuse, tex );
	_baseMaterial->_hasAlpha = tex->GetHasAlpha();
	_baseMaterial->_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	_baseMaterial->UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Normal, tex );
}

void PhysicallyBasedMaterial::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	_baseMaterial->UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Specular, tex );
}

void PhysicallyBasedMaterial::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	_baseMaterial->UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Opacity, tex );

	_baseMaterial->_hasAlpha = tex != nullptr;
	_baseMaterial->_changedAlpha = true;
}