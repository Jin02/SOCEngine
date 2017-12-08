#pragma once

#include <string>
#include <fstream>
#include "ShaderMacro.hpp"
#include "DirectX.h"
#include <functional>
#include <unordered_map>

namespace Rendering
{
	namespace Manager
	{
		struct MakeShaderKeyParam
		{
			std::string folderPath	= "";
			std::string fileName	= "";
			std::string mainFunc	= "";
			std::string shaderType	= "";

			const std::vector<Shader::ShaderMacro>* macros = nullptr;
		};

		template <class ShaderType>
		class ShaderPool
		{
		public:
			using ShaderPtr		= std::shared_ptr<ShaderType>;

		public:
			DISALLOW_ASSIGN(ShaderPool);

			ShaderPool() = default;

			void Add(const std::string& key, const ShaderPtr& shader)
			{
				_shaders.insert(make_pair(key, shader));
			}

			ShaderPtr Find(const std::string& key)
			{
				auto findIter	= _shaders.find(key);
				return findIter != _shaders.end() ? findIter->second : nullptr;
			}
			bool Has(const std::string& key)
			{
				auto findIter = _shaders.find(key);
				return findIter != _shaders.end();
			}
			void DeleteAll()
			{
				for (auto& iter : _shaders)
					iter.second.reset();

				_shaders.clear();
			}
			void Delete(const std::string& key)
			{
				auto findIter = _shaders.find(key);
				if (findIter != _shaders.end())
				{
					findIter->second.reset();
					_shaders.erase(findIter);
				}
			}
			void Destroy()
			{
				DeleteAll();
			}

		private:
			std::unordered_map<std::string, ShaderPtr>		_shaders;
		};
	}
}
