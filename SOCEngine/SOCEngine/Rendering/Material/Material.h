#pragma once

#include "Shaders.h"
#include "Common.h"
#include "Texture2D.h"
#include "Vector2.h"

#include "Container.h"
#include <map>

//dont use const reference
#define SET_ACCESSOR_MATERIAL(name, type)\
	inline void Set##name(const type& data)	{ SetVariable(#name, data);}

//dont use const reference
#define GET_ACCESSOR_MATERIAL(name, type)\
	inline bool Get##name(type& out) const { return GetVariable(out, #name); }

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

		struct CustomShader
		{
			bool isDeferred;
			Shader::ShaderGroup shaderGroup;

			CustomShader();
			~CustomShader();
		};


	private:
		std::string														_name;
		Math::Vector2													_tiling;

		std::vector<Shader::ShaderForm::InputConstBuffer>				_constBuffers;
		std::vector<Shader::ShaderForm::InputTexture>					_textures;
		std::vector<Shader::ShaderForm::InputShaderResourceBuffer>		_srBuffers;

		std::map<const std::string, Container>							_datas;

		const Type														_type;
		uint															_variableUpdateCounter;

	protected:
		CustomShader													_customShaders; //in forward rendering
		bool															_hasAlpha;
		bool															_changedAlpha;

	public:
		Material(const std::string& name, Type type);
		~Material(void);

	public:
		virtual void Initialize();
		virtual void Destroy();
		virtual void UpdateConstBuffer(const Device::DirectX* dx);

	public:
		const Buffer::ConstBuffer* FindConstBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool SetConstBufferUseShaderSlotIndex(uint shaderSlotIdx, const Buffer::ConstBuffer* cb, Rendering::Shader::ShaderForm::Usage usage);
		bool SetConstBufferUseArrayIndex(uint arrayIdx, const Buffer::ConstBuffer* cb, Rendering::Shader::ShaderForm::Usage usage);

	public:
		const Rendering::Texture::Texture2D* FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool SetTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture2D* texture, Rendering::Shader::ShaderForm::Usage usage);
		bool SetTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture2D* texture, Rendering::Shader::ShaderForm::Usage usage);

	public:
		const Rendering::Buffer::ShaderResourceBuffer* FindShaderResourceBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool SetShaderResourceBufferUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, Rendering::Shader::ShaderForm::Usage usage);
		bool SetShaderResourceBufferUseArrayIndex(unsigned int arrayIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, Rendering::Shader::ShaderForm::Usage usage);


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

		GET_SET_ACCESSOR(CustomShader, const CustomShader&, _customShaders);

		GET_ACCESSOR(VariableUpdateCounter, uint, _variableUpdateCounter);

	public:
		GET_ACCESSOR(Textures, const std::vector<Shader::ShaderForm::InputTexture>&, _textures);
		GET_ACCESSOR(ConstBuffers, const std::vector<Shader::ShaderForm::InputConstBuffer>&, _constBuffers);
		GET_ACCESSOR(ShaderResourceBuffers, const std::vector<Shader::ShaderForm::InputShaderResourceBuffer>&, _srBuffers);
	};
}