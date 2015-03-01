#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "Common.h"
#include "Texture.h"
#include "Vector2.h"

namespace Rendering
{
	class Material
	{
	protected:
		std::string		_name;
		Math::Vector2	_tiling;

		bool			_hasAlpha;
		bool			_changedAlpha;

		std::vector<Shader::BaseShader::TextureType> _textures;

	public:
		Material(const std::string& name);
		~Material(void);

	public:
		void Init(const std::vector<unsigned int>& textureShaderSlotIndex);
		const Rendering::Texture::Texture* FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool UpdateTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture);
		bool UpdateTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture* texture);

	public:
		GET_ACCESSOR(Name, const std::string&, _name);

		GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		GET_SET_ACCESSOR(ChangedAlpha, bool, _changedAlpha);
		GET_SET_ACCESSOR(UVTiling, const Math::Vector2&, _tiling);
	};
}