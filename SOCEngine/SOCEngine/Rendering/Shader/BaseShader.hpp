#pragma once

#include <string>
#include "DirectX.h"
#include "Texture2D.h"
#include "ShaderResourceBuffer.h"
#include <vector>
#include "BindIndexInfo.h"
#include "ConstBuffer.h"

#define BASIC_SHADER_NAME "Basic"
#define BASIC_NORMAL_MAPPING_SHADER_NAME "BasicNormalMapping"

#define BASIC_VS_MAIN_FUNC_NAME "VS"
#define BASIC_PS_MAIN_FUNC_NAME "PS"

namespace Rendering
{
	namespace Shader
	{
		class BaseShader final
		{
		public:
			BaseShader(const DXResource<ID3DBlob>& blob, const std::string& key) : _blob(blob), _key(key) {}

			GET_CONST_ACCESSOR(Blob, DXResource<ID3DBlob>, _blob);
			GET_CONST_ACCESSOR(Key, const std::string&, _key);

		private:
			std::string			_key;
			DXResource<ID3DBlob>		_blob;
		};
	}
}
