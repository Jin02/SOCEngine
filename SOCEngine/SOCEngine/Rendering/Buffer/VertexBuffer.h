#pragma once

#include "BaseBuffer.h"
#include "VertexShader.h"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer
		{
		public:
			using Key = uint;

			struct Desc
			{
				Desc() = default;
				Desc(const std::string& _strKey, Key _key, uint _stride, uint _vertexCount)
					: strKey(_strKey), key(_key), stride(_stride), vertexCount(_vertexCount) { }

				std::string		strKey		= "";
				Key				key			= -1;
				uint			stride		= 0;
				uint			vertexCount	= 0;
			};

			VertexBuffer() = default;

			void Initialize(Device::DirectX& dx, const Desc& desc, const void* sysMem, bool isDynamic, const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos);
			void IASetBuffer(Device::DirectX& dx);
			void UpdateVertexData(Device::DirectX& dx, const void* data, uint size);

			GET_CONST_ACCESSOR(VertexCount, uint, _desc.vertexCount);
			GET_CONST_ACCESSOR(Key, Key, _desc.key);
			GET_CONST_ACCESSOR(StrKey, const auto&, _desc.strKey);

			GET_CONST_ACCESSOR(Semantics, const std::vector<Shader::VertexShader::SemanticInfo>&, _semantics);

		private:
			BaseBuffer	_baseBuffer;
			Desc		_desc;

			std::vector<Shader::VertexShader::SemanticInfo>	_semantics; //attributes
		};
	}
}
