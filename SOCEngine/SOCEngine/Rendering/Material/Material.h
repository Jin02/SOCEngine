#pragma once

#include "Shaders.h"
#include "Common.h"
#include "Texture.h"
#include "Vector2.h"

#include "Container.h"
#include <map>

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

		std::vector<Shader::BaseShader::TextureType>	_textures;
		std::map<const std::string, Container>			_datas;

		const Type										_type;

		std::array<Shader::RenderShaders, (uint)Shader::RenderShaderType::Num>	_customShaders;

	public:
		Material(const std::string& name, Type type);
		~Material(void);

	public:
		const Rendering::Texture::Texture* FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex);

		bool UpdateTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture);
		bool UpdateTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture* texture);

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
		}

		template<typename Type>
		bool GetVariable(Type& outContainer, const std::string& name)
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

		GET_SET_ACCESSOR(CustomRenderSceneShader, const Shader::RenderShaders&, _customShaders[(uint)Shader::RenderShaderType::RenderScene]);
		GET_SET_ACCESSOR(CustomDepthWriteShader, const Shader::RenderShaders&, _customShaders[(uint)Shader::RenderShaderType::DepthWrite]);
		GET_SET_ACCESSOR(CustomAlphaTestWithDiffuseShader, const Shader::RenderShaders&, _customShaders[(uint)Shader::RenderShaderType::AlphaTestWithDiffuse]);

		inline const std::array<Shader::RenderShaders, (uint)Shader::RenderShaderType::Num>& GetCustomShader() const { return _customShaders; }
		inline void SetCustomShader(const std::array<Shader::RenderShaders, (uint)Shader::RenderShaderType::Num>& shaders) { _customShaders = shaders; }

		friend class PhysicallyBasedMaterial;
	};
}