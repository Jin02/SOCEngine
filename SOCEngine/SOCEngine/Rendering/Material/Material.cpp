#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

Material::Color::Color() 
	:diffuse(1.0f, 1.0f, 1.0f), ambient(0.5f, 0.5f, 0.5f),
	specular(1.0f, 1.0f, 1.0f), emissive(0.0f, 0.0f, 0.0f),
	shiness(0.0f), opacity(1.0f)
{
}

Material::Color::~Color()
{
}


Material::Material(const std::string& name)	
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _colorBuffer(nullptr),
	_updateConstBufferMethod(UpdateCBMethod::Default),
	_hasAlpha(false), _changedAlpha(true)
{
}

Material::Material(const std::string& name, const Color& color) 
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _color(color), _colorBuffer(nullptr),
	_updateConstBufferMethod(UpdateCBMethod::Default), _changedAlpha(true)
{
	_hasAlpha = color.opacity < 1.0f;
}

Material::~Material(void)
{
	SAFE_DELETE(_colorBuffer);
}

void Material::InitColorBuffer(ID3D11DeviceContext* context)
{	
	auto& psConstBuffers = _constbuffers.usagePS;
	if( psConstBuffers.size() != 0 )
		ASSERT("Error!, ps constbuffer already exists");

	_colorBuffer = new Buffer::ConstBuffer;
	
	// const buffer size certainly is 16 multiple
	_colorBuffer->Create(sizeof(Material::Color) + 8);

	psConstBuffers.push_back(std::make_pair(BasicConstBuffercSlot::MaterialColor, _colorBuffer));	
	UpdateColorBuffer(context);
}

void Material::UpdateColorBuffer(ID3D11DeviceContext* context)
{
	if(_colorBuffer)
		_colorBuffer->Update(context, &_color);
}

void Material::UpdateBasicConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera)
{
	if(_updateConstBufferMethod != UpdateCBMethod::Default)
		return;

	auto SetBufferData = [](std::vector<Shader::BaseShader::BufferType>& buffers, 
		BasicConstBuffercSlot slotType, 
		unsigned int idx,
		const Buffer::ConstBuffer* constBuffer)
	{
		BasicConstBuffercSlot bufferType = static_cast<BasicConstBuffercSlot>(buffers[idx].first);

		if(bufferType == slotType)
			buffers[idx].second = constBuffer;
		else
		{
			ASSERT("vs constbuffer already has another buffer");
			buffers[idx] = std::make_pair(slotType, constBuffer);
		}
	};

	auto& vsBuffers = _constbuffers.usageVS;
	if(vsBuffers.size() == 0)
	{
		vsBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::Transform, transform));
		vsBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::Camera, camera));
	}
	else
	{
		unsigned int idx = Material::BasicConstBuffercSlot::Transform;
		SetBufferData(vsBuffers, Material::BasicConstBuffercSlot::Transform, idx, transform);

		if(vsBuffers.size() == 1)
			vsBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::Camera, camera));
		else if(vsBuffers.size() >= 2)
		{
			idx = Material::BasicConstBuffercSlot::Camera;
			SetBufferData(vsBuffers, Material::BasicConstBuffercSlot::Camera, idx, camera);
		}
	}

	auto& psBuffers = _constbuffers.usagePS;
	if(psBuffers.size() == 0)
	{
		psBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::MaterialColor, nullptr));
		psBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::Camera, camera));
	}
	else
	{
		SetBufferData(psBuffers, Material::BasicConstBuffercSlot::MaterialColor, 0, _colorBuffer);

		if(psBuffers.size() == 1)
			psBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::Camera, camera));
		else if(vsBuffers.size() >= 2)
		{
			SetBufferData(psBuffers, Material::BasicConstBuffercSlot::Camera, 1, camera);
		}
	}
}

const Rendering::Texture::Texture* Material::FindMap(unsigned int& outIndex, unsigned int shaderSlotIndex)
{
	auto textures = _textures.usagePS;
	for(unsigned int i=0; i<textures.size(); ++i)
	{
		if(textures[i].first == shaderSlotIndex)
		{
			outIndex = i;
			return textures[i].second;
		}
	}

	outIndex = 0;
	return nullptr;
}

void Material::UpdateMap(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture)
{
	unsigned int index = 0;
	auto map = FindMap(index, shaderSlotIndex);
	if(map == nullptr)
		_textures.usagePS.push_back(std::make_pair(shaderSlotIndex, texture));
	else
	{
		_textures.usagePS[index].second = texture;
	}
}

void Material::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	UpdateMap(TextureType::Diffuse, tex);
	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void Material::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	UpdateMap(TextureType::Normal, tex);
}

void Material::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	UpdateMap(TextureType::Specular, tex);
}

void Material::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	UpdateMap(TextureType::Opacity, tex);
	_hasAlpha = tex != nullptr;
	_changedAlpha = true;
}

void Material::UpdateAmbientMap(const Rendering::Texture::Texture* tex)
{
	UpdateMap(TextureType::Ambient, tex);
}

void Material::ClearResource(ID3D11DeviceContext* context)
{
	_vertexShader->ClearResource(context, &_textures.usageVS);
	_pixelShader->ClearResource(context, &_textures.usagePS);
}

void Material::UpdateShader(ID3D11DeviceContext* context)
{
	_vertexShader->UpdateShader(context);
	_pixelShader->UpdateShader(context);
}

void Material::UpdateResources(ID3D11DeviceContext* context)
{
	_vertexShader->UpdateResources(context, &_constbuffers.usageVS, &_textures.usageVS);

	Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
	std::vector<Shader::PixelShader::SamplerType> samplers;

	//임시로, 0번에 linear만 넣음.
	samplers.push_back(std::make_pair(0, sampler));


	_pixelShader->UpdateResources(context, &_constbuffers.usagePS, &_textures.usagePS, samplers);
}

void Material::UpdateColor(const Color& color)
{
	_color = color;
	_hasAlpha = (_color.opacity < 1.0f);
	_changedAlpha = true;
}