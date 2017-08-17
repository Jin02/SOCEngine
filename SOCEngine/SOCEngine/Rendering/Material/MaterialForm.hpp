#pragma once

#include "Shaders.h"
#include "Common.h"
#include "ConstBuffer.h"
#include "Texture2D.h"
#include "ShaderResourceBuffer.h"
#include "VectorIndexer.hpp"

namespace Rendering
{
	class MaterialForm
	{
	public:
		using BindConstBuffer = Shader::BindShaderData<Buffer::ConstBuffer>;
		using BindTextured2D = Shader::BindShaderData<Texture::Texture2D>;
		using BindSRBuffer = Shader::BindShaderData<Buffer::ShaderResourceBuffer>;

		MaterialForm(const std::string& name) : _name(name) {}

		GET_CONST_ACCESSOR(Name, const std::string&, _name);
		GET_ACCESSOR(Textures, auto&, _textures);
		GET_ACCESSOR(ConstBuffers, auto&, _constBuffers);
		GET_ACCESSOR(ShaderResourceBuffers, auto&, _srBuffers);

	protected:
		using Texture2Ds = Core::VectorHashMap<std::string, BindTextured2D>;
		using ConstBuffers = Core::VectorHashMap<std::string, BindConstBuffer>;
		using SRBuffers = Core::VectorHashMap<std::string, BindSRBuffer>;
		GET_ACCESSOR(TextureBook, Texture2Ds&, _textures);
		GET_ACCESSOR(ConstBufferBook, ConstBuffers&, _constBuffers);
		GET_ACCESSOR(SRBufferBook, SRBuffers&, _srBuffers);

	private:
		ConstBuffers	_constBuffers;
		Texture2Ds		_textures;
		SRBuffers		_srBuffers;

		std::string		_name	= "";
		float			_alpha	= 1.0f;
	};
}
