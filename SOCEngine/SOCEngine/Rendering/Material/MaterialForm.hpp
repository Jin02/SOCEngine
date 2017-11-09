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
		using BindTextured2D	= Shader::BindShaderData<Texture::Texture2D>;
		using Texture2Ds		= Core::VectorHashMap<std::string, BindTextured2D>;

		using BindSRBuffer		= Shader::BindShaderData<Buffer::ShaderResourceBuffer>;
		using SRBuffers			= Core::VectorHashMap<std::string, BindSRBuffer>;

		using BindConstBuffer	= Shader::BindShaderData<Buffer::ConstBuffer>;
		using ConstBuffers		= Core::VectorHashMap<std::string, BindConstBuffer>;

		MaterialForm(const std::string& name) : _name(name) {}

		GET_CONST_ACCESSOR(Name,					const std::string&,	_name);
		GET_CONST_ACCESSOR(Textures,				const auto&,		_textures);
		GET_CONST_ACCESSOR(ConstBuffers,			const auto&,		_constBuffers);
		GET_CONST_ACCESSOR(ShaderResourceBuffers,	const auto&,		_srBuffers);

	protected:
		GET_ACCESSOR(Textures,						Texture2Ds&,		_textures);
		GET_ACCESSOR(ConstBuffers,					ConstBuffers&,		_constBuffers);
		GET_ACCESSOR(ShaderResourceBuffers,			SRBuffers&,			_srBuffers);

	private:
		ConstBuffers	_constBuffers;
		Texture2Ds		_textures;
		SRBuffers		_srBuffers;

		std::string		_name	= "";
		float			_alpha	= 1.0f;
	};
}
