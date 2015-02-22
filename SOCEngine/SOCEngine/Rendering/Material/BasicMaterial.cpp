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

BasicMaterial::BasicMaterial(const std::string& name, const Color& color) :Material(name),
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

	// Basic Slot Setting
	{
		unsigned int count = static_cast<unsigned int>( VSConstBufferSlot::COUNT );
		std::vector<unsigned int> basicVSCBSlotIndices(count);
		{
			unsigned int idx = static_cast<unsigned int>(VSConstBufferSlot::Camera);
			basicVSCBSlotIndices[idx] = idx;

			idx = static_cast<unsigned int>(VSConstBufferSlot::Transform);
			basicVSCBSlotIndices[idx] = idx;
		}

		count = static_cast<unsigned int>( PSConstBufferSlot::COUNT );
		std::vector<unsigned int> basicPSCBSlotIndices(count);
		{
			unsigned int idx = static_cast<unsigned int>(PSConstBufferSlot::Camera);
			basicPSCBSlotIndices[idx] = idx;

			idx = static_cast<unsigned int>( PSConstBufferSlot::MaterialColor );
			basicPSCBSlotIndices[idx] = idx; 
		}

		count = static_cast<unsigned int>( PSTextureSlot::COUNT );
		std::vector<unsigned int> samplePSTextureIndices(count);
		{
			unsigned int idx = static_cast<unsigned int>(PSTextureSlot::Diffuse);
			samplePSTextureIndices[idx] = static_cast<unsigned int>(TextureType::Diffuse);

			idx = static_cast<unsigned int>( PSTextureSlot::Normal );
			samplePSTextureIndices[idx] = static_cast<unsigned int>(TextureType::Normal); 
		}

		std::vector<unsigned int> sampleNullVsTextureIndices;
		Material::Init(basicVSCBSlotIndices,		basicPSCBSlotIndices, 
					   sampleNullVsTextureIndices,	samplePSTextureIndices);
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
		unsigned int idx = static_cast<unsigned int>( PSConstBufferSlot::MaterialColor );
		PSConstBufferSlot bufferType = static_cast<PSConstBufferSlot>(psBuffers[idx].first);

		bool condition = (bufferType == PSConstBufferSlot::MaterialColor);
		ASSERT_COND_MSG(condition, "ps constbuffer already has another buffer");
		{
			psBuffers[idx].second = _colorBuffer;
		}
	}
}

void BasicMaterial::UpdateColor(const Color& color)
{
	_color = color;
	_hasAlpha = (_color.main.a < 1.0f);
	_changedAlpha = true;
}

void BasicMaterial::UpdateBasicConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera)
{
	if( (_updateConstBufferMethod != UpdateCBMethod::Default) && (_isInit == false) )
		return;

	auto& vsBuffers = _constbuffers.usageVS;
	{
		auto SetBufferData = []( std::vector<Shader::BaseShader::BufferType>& buffers, VSConstBufferSlot slotType, unsigned int idx, const Buffer::ConstBuffer* constBuffer)
		{
			VSConstBufferSlot bufferType = static_cast<VSConstBufferSlot>(buffers[idx].first);

			ASSERT_COND_MSG(bufferType == slotType, "vs constbuffer already has another buffer");
			{
				buffers[idx].second = constBuffer;
			}
		};

		// Transform
		{
			unsigned int idx = static_cast<unsigned int>( VSConstBufferSlot::Transform );
			SetBufferData(vsBuffers, VSConstBufferSlot::Transform, idx, transform);
		}

		// Camera
		{
			unsigned idx = static_cast<unsigned int>( VSConstBufferSlot::Camera );
			SetBufferData(vsBuffers, VSConstBufferSlot::Camera, idx, camera);
		}
	}

	auto& psBuffers = _constbuffers.usagePS;
	{
		unsigned int idx = static_cast<unsigned int>( PSConstBufferSlot::Camera );
		PSConstBufferSlot bufferType = static_cast<PSConstBufferSlot>(psBuffers[idx].first);

		ASSERT_COND_MSG(bufferType == PSConstBufferSlot::Camera, "ps constbuffer already has another buffer");
		{
			psBuffers[idx].second = camera;
		}
	}
}

void BasicMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	auto& psTextures = _textures.usagePS;
	{
		unsigned int idx = static_cast<unsigned int>( PSTextureSlot::Diffuse );
		TextureType texType = static_cast<TextureType>(psTextures[idx].first);

		ASSERT_COND_MSG(texType == TextureType::Diffuse, "ps texture already has another tex");
		{
			psTextures[idx].second = tex;
		}
	}

	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void BasicMaterial::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	auto& psTextures = _textures.usagePS;
	{
		unsigned int idx = static_cast<unsigned int>( PSTextureSlot::Normal );
		TextureType texType = static_cast<TextureType>(psTextures[idx].first);

		ASSERT_COND_MSG(texType == TextureType::Normal, "ps texture already has another tex");
		{
			psTextures[idx].second = tex;
		}
	}
}