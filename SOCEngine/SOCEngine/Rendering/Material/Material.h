#pragma once

#include "Shaders.h"
#include "Common.h"
#include "Texture.h"
#include "Vector2.h"

#include "Container.h"
#include <map>

//dont use const reference
#define SET_ACCESSOR_MATERIAL(name, type) inline void Set##name(const type& data)			{ SetVariable(#name, data);}

//dont use const reference
#define GET_ACCESSOR_MATERIAL(name, type) inline bool Get##name(type& out) const			{ return GetVariable(out, #name); }

//dont use const reference
#define GET_SET_ACCESSOR_MATERIAL(name, type)\
	SET_ACCESSOR_MATERIAL(name, type) GET_ACCESSOR_MATERIAL(name, type)

namespace Rendering
{
	class Material
	{
	public:
		enum class Type : unsigned int
		{
			UnknownModel,
			PhysicallyBasedModel,
			UI,
			Users
		};

	protected:
		std::string										_name;
		Math::Vector2									_tiling;

		bool											_hasAlpha;
		bool											_changedAlpha;

		std::vector<Shader::BaseShader::BufferType>		_constBuffers;
		std::vector<Shader::BaseShader::TextureType>	_textures;
		std::map<const std::string, Container>			_datas;

		const Type										_type;

		Shader::ShaderGroup								_customShaders; //in forward rendering

		uint											_variableUpdateCounter;

	public:
		Material(const std::string& name, Type type);
		~Material(void);

	public:
		const Buffer::ConstBuffer* FindConstBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool UpdateConstBuffer_ShaderSlotIndex(uint shaderSlotIdx, const Buffer::ConstBuffer* cb);
		bool UpdateConstBuffer_ArrayIndex(uint arrayIdx, const Buffer::ConstBuffer* cb);

	public:
		const Rendering::Texture::Texture* FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool UpdateTexture_ShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture);
		bool UpdateTexture_ArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture* texture);

	public:
		template<typename Type>
		void SetVariable(const std::string& name, const Type& value)
		{
			auto findIter = _datas.find(name);
			if( findIter == _datas.end() )
			{
				{
					Container container;
					container.Init<Type>(value);
					container.SetEnableDealloc(false);

					_datas.insert(std::make_pair(name, container));
				}

				auto reFindIter = _datas.find(name);
				reFindIter->second.SetEnableDealloc(true);
			}
			else
			{
				findIter->second.SetData(value);
			}

			++_variableUpdateCounter;
		}

		template<typename Type>
		bool GetVariable(Type& outContainer, const std::string& name) const
		{
			auto findIter = _datas.find(name);
			if( findIter == _datas.end() )
				return false;

			outContainer = findIter->second.GetData<Type>();
			return true;
		}

	public:
		GET_ACCESSOR(Name, const std::string&, _name);

		GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		GET_ACCESSOR(Type, Type, _type);

		GET_SET_ACCESSOR(ChangedAlpha, bool, _changedAlpha);
		GET_SET_ACCESSOR(UVTiling, const Math::Vector2&, _tiling);

		GET_SET_ACCESSOR(CustomShader, const Shader::ShaderGroup&, _customShaders);

	public:
		GET_ACCESSOR(Textures, const std::vector<Shader::BaseShader::TextureType>&, _textures);
		GET_ACCESSOR(ConstBuffers, const std::vector<Shader::BaseShader::BufferType>&, _constBuffers);
	};
}