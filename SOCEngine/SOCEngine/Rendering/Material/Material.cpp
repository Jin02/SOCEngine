#include "Material.h"
#include "Utility.h"

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
	_updateConstBufferMethod(UpdateCBMethod::Default)
{
}

Material::Material(const std::string& name, const Color& color) 
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _color(color), _colorBuffer(nullptr),
	_updateConstBufferMethod(UpdateCBMethod::Default)
{

}

Material::~Material(void)
{
	SAFE_DELETE(_colorBuffer);
}

void Material::InitColorBuffer(ID3D11DeviceContext* context)
{
	Buffer::ConstBuffer* colorBuffer = new Buffer::ConstBuffer;

	// const buffer size certainly is 16 multiple
	colorBuffer->Create(sizeof(Material::Color) + 8);
	
	auto& psConstBuffers = _constbuffers.usagePS;
	if( psConstBuffers.size() != 0 )
		ASSERT("Error!, ps constbuffer already exists");

	psConstBuffers.push_back(std::make_pair(BasicConstBuffercSlot::MaterialColor, colorBuffer));	
	_colorBuffer = dynamic_cast<Buffer::ConstBuffer*>(psConstBuffers[0].second);

	UpdateColorBuffer(context);
}

void Material::UpdateColorBuffer(ID3D11DeviceContext* context)
{
	if(_colorBuffer)
		_colorBuffer->Update(context, &_color);
}

void Material::UpdateTransformBuffer(ID3D11DeviceContext* context, Buffer::ConstBuffer* transform)
{
	if(_updateConstBufferMethod != UpdateCBMethod::Default)
		return;
	
	auto& vsBuffers = _constbuffers.usageVS;
	if(vsBuffers.size() == 0)
	{
		vsBuffers.push_back(std::make_pair(Material::BasicConstBuffercSlot::Transform, transform));
	}
	else
	{
		if(vsBuffers[0].first == Material::BasicConstBuffercSlot::Transform)
			vsBuffers[0].second = transform;
		else
		{
			DEBUG_LOG("vs constbuffer already has another buffer");
			vsBuffers[0] = std::make_pair(Material::BasicConstBuffercSlot::Transform, transform);
		}
	}
}

bool Material::UpdateTexture(unsigned int index, const Rendering::Texture::Texture* texture)
{
	_textures.usagePS.push_back(std::make_pair(index, texture));
	return true;
}

void Material::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Diffuse, tex) == false)
		_textures.usagePS.push_back(std::make_pair(TextureType::Diffuse, tex));
}

void Material::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Normal, tex) == false)
		_textures.usagePS.push_back(std::make_pair(TextureType::Normal, tex));
}

void Material::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Specular, tex) == false)
		_textures.usagePS.push_back(std::make_pair(TextureType::Specular, tex));
}

void Material::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Opacity, tex) == false)
		_textures.usagePS.push_back(std::make_pair(TextureType::Opacity, tex));
}

void Material::UpdateAmbientMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Ambient, tex) == false)
		_textures.usagePS.push_back(std::make_pair(TextureType::Ambient, tex));
}

void Material::ClearResource(ID3D11DeviceContext* context)
{
	_vertexShader->ClearResource(context, &_textures.usageVS);
	_pixelShader->ClearResource(context, &_textures.usagePS);
}

void Material::UpdateShader(ID3D11DeviceContext* context, const std::vector<Shader::PixelShader::SamplerType>& samplers)
{
	_vertexShader->UpdateShader(context, &_constbuffers.usageVS, &_textures.usageVS);
	_pixelShader->UpdateShader(context, &_constbuffers.usagePS, &_textures.usagePS, samplers);
}
