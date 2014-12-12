#include "Material.h"
#include "Utility.h"

using namespace Rendering::Material;
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
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _colorBuffer(nullptr)
{
}

Material::Material(const std::string& name, const Color& color) 
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _color(color), _colorBuffer(nullptr)
{

}

Material::~Material(void)
{
	SAFE_DELETE(_colorBuffer);
}

void Material::InitColorBuffer(ID3D11DeviceContext* context)
{
	Buffer::ConstBuffer* colorBuffer = new Buffer::ConstBuffer;
	colorBuffer->Create(sizeof(Material::Color), 0, ConstBuffer::Usage::PixelShader);
	_constBuffer.push_back(std::make_pair(BasicConstBuffercSlot::MaterialColor, colorBuffer));	
	_colorBuffer = dynamic_cast<Buffer::ConstBuffer*>(_constBuffer[0].second);

	UpdateColorBuffer(context);
}

void Material::UpdateColorBuffer(ID3D11DeviceContext* context)
{
	if(_colorBuffer)
		_colorBuffer->UpdateSubresource(context, &_color);
}

bool Material::UpdateTexture(unsigned int index, const Rendering::Texture::Texture* texture)
{
	_textures.push_back(std::make_pair(index, texture));
	return true;
}

void Material::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Diffuse, tex) == false)
		_textures.push_back(std::make_pair(TextureType::Diffuse, tex));
}

void Material::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Normal, tex) == false)
		_textures.push_back(std::make_pair(TextureType::Normal, tex));
}

void Material::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Specular, tex) == false)
		_textures.push_back(std::make_pair(TextureType::Specular, tex));
}

void Material::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Opacity, tex) == false)
		_textures.push_back(std::make_pair(TextureType::Opacity, tex));
}

void Material::UpdateAmbientMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Ambient, tex) == false)
		_textures.push_back(std::make_pair(TextureType::Ambient, tex));
}