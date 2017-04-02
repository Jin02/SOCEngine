#pragma once

#include "Shaders.h"
#include "Common.h"
#include "ConstBuffer.h"
#include "Texture2D.h"
#include "ShaderResourceBuffer.h"
#include "VectorIndexer.hpp"

namespace Rendering
{
	class Material
	{
	public:
		using BindConstBuffer = Shader::BindShaderData<Buffer::ConstBuffer>;
		using BindTextured2D = Shader::BindShaderData<Texture::Texture2D>;
		using BindSRBuffer = Shader::BindShaderData<Buffer::ShaderResourceBuffer>;

		Material(const std::string& name) : _name(name) {}

		GET_CONST_ACCESSOR(Name, const std::string&, _name);
		GET_CONST_ACCESSOR(Textures, const std::vector<BindTextured2D>&, _textures.GetVector());
		GET_CONST_ACCESSOR(ConstBuffers, const std::vector<BindConstBuffer>&, _constBuffers.GetVector());
		GET_CONST_ACCESSOR(ShaderResourceBuffers, const std::vector<BindSRBuffer>&, _srBuffers.GetVector());

	protected:
		using Texture2Ds = Core::VectorHashMap<std::string, BindTextured2D>;
		using ConstBuffers = Core::VectorHashMap<std::string, BindConstBuffer>;
		using SRBuffers = Core::VectorHashMap<std::string, BindSRBuffer>;
		GET_ACCESSOR(TextureBook, Texture2Ds&, _textures);
		GET_ACCESSOR(ConstBufferBook, ConstBuffers&, _constBuffers);
		GET_ACCESSOR(SRBufferBook, SRBuffers&, _srBuffers);

	private:
		ConstBuffers		_constBuffers;
		Texture2Ds		_textures;
		SRBuffers		_srBuffers;

		std::string		_name	= "";
		float			_alpha	= 1.0f;
	};
}
