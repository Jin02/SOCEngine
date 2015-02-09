#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

Material::Material(const std::string& name)	
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name),
	_updateConstBufferMethod(UpdateCBMethod::Default),
	_hasAlpha(false), _changedAlpha(true), _isInit(false)
{
}

Material::~Material(void)
{

}

void Material::Init(const std::vector<unsigned int>& vsConstBufferUsageIndices, const std::vector<unsigned int >& psConstBufferUsageIndices)
{
	for(auto& idx : vsConstBufferUsageIndices)
		_constbuffers.usageVS.push_back( std::make_pair(idx, nullptr) );

	for(auto& idx : psConstBufferUsageIndices)
		_constbuffers.usagePS.push_back( std::make_pair(idx, nullptr) );

	_isInit = true;
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