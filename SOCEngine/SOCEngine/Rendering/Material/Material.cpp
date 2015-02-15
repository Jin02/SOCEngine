#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

Material::Material(const std::string& name)	
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name),
	_updateConstBufferMethod(UpdateCBMethod::Default),
	_hasAlpha(false), _changedAlpha(true)
{
}

Material::~Material(void)
{

}

void Material::Init(const std::vector<unsigned int>& vsConstBufferUsageIndices, const std::vector<unsigned int >& psConstBufferUsageIndices,
					const std::vector<unsigned int>& vsTextureUsageIndices,		const std::vector<unsigned int>& psTextureUsageIndices)
{
	InitConstBufferSlot(vsConstBufferUsageIndices, psConstBufferUsageIndices);
	InitTextureSlot(vsTextureUsageIndices, psTextureUsageIndices);
}

void Material::InitConstBufferSlot(const std::vector<unsigned int>& vsTextureUsageIndices, const std::vector<unsigned int >& psTextureUsageIndices)
{
	for(auto& idx : vsTextureUsageIndices)
		_constbuffers.usageVS.push_back( std::make_pair(idx, nullptr) );

	for(auto& idx : psTextureUsageIndices)
		_constbuffers.usagePS.push_back( std::make_pair(idx, nullptr) );
}

void Material::InitTextureSlot(const std::vector<unsigned int>& vsShaderSlotIndex, const std::vector<unsigned int>& psShaderSlotIndex)
{
	for(auto& idx : vsShaderSlotIndex)
		_textures.usageVS.push_back(  std::make_pair(idx, nullptr) );

	for(auto& idx : psShaderSlotIndex)
		_textures.usagePS.push_back(  std::make_pair(idx, nullptr) );
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