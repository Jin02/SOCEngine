#pragma once

#include <string>
#include <fstream>
#include "ShaderMacro.h"
#include "DirectX.h"
#include <functional>
#include <unordered_map>

namespace Rendering
{
	namespace Manager
	{
		struct MakeShaderKeyParam
		{
			std::string folderPath;
			std::string fileName;
			std::string mainFunc;
			std::string shaderType;

			const std::vector<Shader::ShaderMacro>* macros;
		};

		template <class ShaderType>
		class ShaderPool
		{
		public:
			using ShaderPtr		= std::shared_ptr<ShaderType>;
			using MakeKeyFunc	= std::function<std::string(const MakeShaderKeyParam& param)>;
			using CreateBlobFunc= std::function<DXResource<ID3DBlob>(const MakeShaderKeyParam& param, bool useRecycle)>;

		public:
			DISALLOW_ASSIGN(ShaderPool);

			ShaderPool()
				: _shaders(), _makeFunc(nullptr), _createBlobFunc(nullptr)
			{

			}
			ShaderPool(const MakeKeyFunc& makeKey, const CreateBlobFunc& createBlob)
				: _shaders(), _makeFunc(makeKey), _createBlobFunc(createBlob)	{ }

			void Add(const std::string& key, const ShaderPtr& shader)
			{
				_shaders.insert(make_pair(key, shader));
			}
			ShaderPtr LoadShader(Device::DirectX& dx, const std::string& folderPath, const std::string& fileName, const std::string& mainFunc, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros, const std::string* uniqueKey)
			{
				MakeShaderKeyParam param;
				{
					param.folderPath	= folderPath;
					param.fileName		= fileName;
					param.mainFunc		= mainFunc;
					param.shaderType	= ShaderType::GetCompileCode();
					param.macros		= macros;
				}
				std::string key = (uniqueKey == nullptr) ? _makeFunc(param) : (*uniqueKey);

				ShaderPtr shader = nullptr;
				if (Has(key) == false)
				{
					DXResource<ID3DBlob> blob = _createBlobFunc(param, useRecycle);
					shader = std::make_shared<ShaderType>(blob, key);
					shader->Initialize(dx);

					_shaders.insert(std::make_pair(key, shader));
				}

				return shader;
			}
			bool FindShader(ShaderPtr& out, const std::string& key)
			{
				auto findIter = _shaders.find(key);
				bool success = findIter != _shaders.end();
				if (success)
					out = findIter->second;

				return success;
			}
			bool Has(const std::string& key)
			{
				auto findIter = _shaders.find(key);
				return findIter != _shaders.end();
			}
			void DeleteAllShader()
			{
				for (auto& iter : _shaders)
					iter.second.reset();

				_shaders.clear();
			}
			void DeleteShader(const std::string& key)
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
				DeleteAllShader();
			}

		public:
			SET_ACCESSOR(MakeKeyFunc, const MakeKeyFunc&, _makeFunc);
			GET_CONST_ACCESSOR(MakeKeyFunc, const MakeKeyFunc&, _makeFunc);

			SET_ACCESSOR(CreateBlobFunc, const CreateBlobFunc&, _createBlobFunc);
			GET_CONST_ACCESSOR(CreateBlobFunc, const CreateBlobFunc&, _createBlobFunc);

		private:
			std::unordered_map<std::string, ShaderPtr>		_shaders;
			MakeKeyFunc										_makeFunc;
			CreateBlobFunc									_createBlobFunc;
		};
	}
}