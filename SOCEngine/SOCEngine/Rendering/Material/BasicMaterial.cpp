#include "BasicMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

BasicMaterial::Color::Color() 
	:main(1.0f, 1.0f, 1.0f, 1.0f), specular(1.0f, 1.0f, 1.0f, 1.0f)
{
}

BasicMaterial::Color::~Color()
{
}


BasicMaterial::BasicMaterial(const std::string& name) 
	: Material(name), _isInit(false)
{
}

BasicMaterial::BasicMaterial(const std::string& name, const Color& color) 
	: Material(name),
	_color(color)
{
	_hasAlpha = _color.main.a < 1.0f;
}

BasicMaterial::~BasicMaterial(void)
{
	SAFE_DELETE(_colorBuffer);
}

void BasicMaterial::Init(ID3D11DeviceContext* context)
{
	auto& psConstBuffers = _constbuffers.usagePS;
	if( psConstBuffers.size() != 0 )
		ASSERT_MSG("Error!, ps constbuffer already exists");

	_colorBuffer = new Buffer::ConstBuffer;	
	_colorBuffer->Create(sizeof(BasicMaterial::Color));

	// Slot Setting
	{
		std::vector<unsigned int> psConstBufferSlotIndices;
		psConstBufferSlotIndices.push_back( (uint)PSConstBufferSlot::MaterialColor );

		std::vector<unsigned int> psTextureIndices;
		psTextureIndices.push_back( (uint)PSTextureSlot::Diffuse );
		psTextureIndices.push_back( (uint)PSTextureSlot::Normal );
		psTextureIndices.push_back( (uint)PSTextureSlot::Specular );
		psTextureIndices.push_back( (uint)PSTextureSlot::Opacity );
		psTextureIndices.push_back( (uint)PSTextureSlot::Ambient );

		std::vector<unsigned int> empty;
		Material::Init(empty,	psConstBufferSlotIndices, 
					   empty,	psTextureIndices);

		_colorConstBufferIdx = _constbuffers.usagePS.size() - 1;
		_isInit = true;
	}

	UpdateColorBuffer(context);
}

void BasicMaterial::UpdateColorBuffer(ID3D11DeviceContext* context)
{
	if( (_colorBuffer == nullptr) && (_isInit == false) )
		return;

	_colorBuffer->Update(context, &_color);

	auto& psBuffers = _constbuffers.usagePS;
	{
		// material color slot index
		const uint index = _colorConstBufferIdx;

		PSConstBufferSlot bufferType = (PSConstBufferSlot)psBuffers[index].first;
		bool condition = (bufferType == PSConstBufferSlot::MaterialColor);
		{
			ASSERT_COND_MSG(condition, "ps constbuffer already has another buffer");
			psBuffers[index].second = _colorBuffer;
		}
	}
}

void BasicMaterial::UpdateColor(const Color& color)
{
	_color = color;
	_hasAlpha = (_color.main.a < 1.0f);
	_changedAlpha = true;
}

void BasicMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Diffuse, tex, UsageTextureType::Pixel );
	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void BasicMaterial::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Normal, tex, UsageTextureType::Pixel );
}

void BasicMaterial::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Specular, tex, UsageTextureType::Pixel );
}

void BasicMaterial::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Opacity, tex, UsageTextureType::Pixel );

	_hasAlpha = tex != nullptr;
	_changedAlpha = true;
}

void BasicMaterial::UpdateAmbientMap(const Rendering::Texture::Texture* tex)
{
	UpdateTextureUseShaderSlotIndex( (uint)PSTextureSlot::Ambient, tex, UsageTextureType::Pixel );
}