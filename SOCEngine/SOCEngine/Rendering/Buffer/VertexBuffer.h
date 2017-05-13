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
			struct Desc
			{
				Desc() = default;
				Desc(const std::string& _key, uint _stride, uint _vertexCount)
					: key(_key), stride(_stride), vertexCount(_vertexCount) { }

				std::string	key		= "";
				uint		stride		= 0;
				uint		vertexCount	= 0;
			};

			VertexBuffer() = default;

			void Initialize(Device::DirectX& dx, const Desc& desc, const void* sysMem, bool isDynamic, const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos);
			void IASetBuffer(Device::DirectX& dx);
			void UpdateVertexData(Device::DirectX& dx, const void* data, uint size);

			GET_CONST_ACCESSOR(VertexCount, uint, _desc.vertexCount);
			GET_CONST_ACCESSOR(Key, const std::string&, _desc.key);

			GET_CONST_ACCESSOR(Semantics, const std::vector<Shader::VertexShader::SemanticInfo>&, _semantics);

		private:
			BaseBuffer	_baseBuffer;
			Desc		_desc;

			std::vector<Shader::VertexShader::SemanticInfo>	_semantics; //attributes
		};
	}
}
