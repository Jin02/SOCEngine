#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;

Material::CustomShader::CustomShader() : isDeferred(false), shaderGroup()
{
}

Material::CustomShader::~CustomShader()
{
}

Material::Material(const std::string& name, Type type)	
	: _name(name), _hasAlpha(false), _changedAlpha(true),
	_tiling(1.0f, 1.0f), _type(type), _variableUpdateCounter(0)
{
}

Material::~Material(void)
{

}

void Material::Initialize(){}
void Material::Destroy(){}
void Material::UpdateConstBuffer(const Device::DirectX* dx){}

const Texture2D* Material::FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{	
	for(unsigned int i=0; i<_textures.size(); ++i)
	{		
		if(_textures[i].semanticIndex == shaderSlotIndex)
		{
			outArrayIndex = i;
			return _textures[i].texture;
		}
	}

	outArrayIndex = -1;
	return nullptr;
}

bool Material::SetTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture2D* texture, ShaderForm::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto hasTexture = FindTexture(arrayIdx, shaderSlotIndex);

	if(hasTexture)
	{
		_textures[arrayIdx].texture = texture;
	}
	else
	{
		_textures.push_back(ShaderForm::InputTexture(shaderSlotIndex, texture, usage));
	}

	return hasTexture != nullptr;
}

bool Material::SetTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture2D* texture, ShaderForm::Usage usage)
{
	if(arrayIndex >= _textures.size())
		return false;

	_textures[arrayIndex].texture = texture;
	_textures[arrayIndex].SetUsage(usage);
	return true;
}

bool Material::SetConstBufferUseShaderSlotIndex(uint shaderSlotIdx, const Buffer::ConstBuffer* cb, ShaderForm::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, shaderSlotIdx);

	if(has)
	{
		_constBuffers[arrayIdx].buffer = cb;
	}
	else
	{
		_constBuffers.push_back(ShaderForm::InputConstBuffer(shaderSlotIdx, cb, usage));
	}

	return has != nullptr;
}

bool Material::SetConstBufferUseArrayIndex(uint arrayIdx, const Buffer::ConstBuffer* cb, ShaderForm::Usage usage)
{
	if(arrayIdx >= _constBuffers.size())
		return false;

	_constBuffers[arrayIdx].buffer = cb;
	_constBuffers[arrayIdx].SetUsage(usage);
	return true;
}

const Buffer::ConstBuffer* Material::FindConstBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{
	for(unsigned int i=0; i<_constBuffers.size(); ++i)
	{		
		if(_constBuffers[i].semanticIndex == shaderSlotIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const Buffer::ConstBuffer*>(_constBuffers[i].buffer);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}

const ShaderResourceBuffer* Material::FindShaderResourceBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{
	for(unsigned int i=0; i<_srBuffers.size(); ++i)
	{		
		if(_srBuffers[i].semanticIndex == shaderSlotIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const ShaderResourceBuffer*>(_srBuffers[i].srBuffer);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}

bool Material::SetShaderResourceBufferUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, ShaderForm::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, shaderSlotIndex);

	if(has)
	{
		_srBuffers[arrayIdx].srBuffer = srBuffer;
	}
	else
	{
		_srBuffers.push_back(ShaderForm::InputShaderResourceBuffer(shaderSlotIndex, srBuffer, usage));
	}

	return has != nullptr;
}

bool Material::SetShaderResourceBufferUseArrayIndex(unsigned int arrayIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, ShaderForm::Usage usage)
{
	if(arrayIndex >= _srBuffers.size())
		return false;

	_srBuffers[arrayIndex].srBuffer = srBuffer;
	_srBuffers[arrayIndex].SetUsage(usage);

	return true;
}