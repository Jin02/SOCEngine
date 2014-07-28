#pragma once

#include <string>
#include "DirectX.h"

namespace Rendering
{
	namespace Shader
	{
		typedef std::string ShaderCode;

		class Shader
		{
		protected:
			ID3DBlob*		_blob;

		public:
			Shader() : _blob(nullptr)
			{
			}

			~Shader(void)
			{
			}

		public:
			static bool CompileFromMemory(ID3DBlob** outBlob, const std::string &shaderCode, const char* shaderModel, const char* funcName)
			{
				DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
				dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

				ID3DBlob* pErrorBlob = nullptr;

				HRESULT hr = D3DX11CompileFromMemory(
					shaderCode.data(), shaderCode.size(),
					nullptr, nullptr, nullptr, funcName,
					shaderModel, dwShaderFlags, 0, nullptr,
					outShader, &pErrorBlob, nullptr);

				if( FAILED(hr) )
				{
					if( pErrorBlob != NULL )
						OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
					if( pErrorBlob ) pErrorBlob->Release();

					return false;
				}
				if( pErrorBlob )
					pErrorBlob->Release();

				return true;
			}
			static bool CompileFromFile(ID3DBlob** outBlob, const std::string &fileName, const char* shaderModel, const char* funcName)
			{
				HRESULT hr = S_OK;

				DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
				// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
				// Setting this flag improves the shader debugging experience, but still allows 
				// the shaders to be optimized and to run exactly the way they will run in 
				// the release configuration of this program.
				dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

				ID3DBlob* pErrorBlob;
				hr = D3DX11CompileFromFile( fileName.data(), NULL, NULL, funcName, shaderModel, 
					dwShaderFlags, 0, NULL, outShader, &pErrorBlob, NULL );
				if( FAILED(hr) )
				{
					if( pErrorBlob != NULL )
						OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
					if( pErrorBlob ) pErrorBlob->Release();

					return false;
				}
				if( pErrorBlob ) pErrorBlob->Release();

				return true;
			}

		public:
			bool CompileFromMemory(const std::string &shaderCode, const char* shaderModel, const char* funcName)
			{
				return CompileFromMemory(&_blob, shaderCode, shaderModel, funcName);
			}
			bool CompileFromFile(const std::string &fileName, const char* shaderModel, const char* funcName)
			{
				return CompileFromFile(&_blob, fileName, shaderModel, funcName);
			}

			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
		};
	}
}
