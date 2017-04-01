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
			struct Param
			{
				Param(const std::string& key = "", uint stride = 0, uint vertexCount = 0);

				std::string			key;
				uint				stride;
				uint				vertexCount;
			};

			VertexBuffer();

			void Initialize(Device::DirectX& dx, const Param& param, const void* sysMem, bool isDynamic, const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos);
			void IASetBuffer(Device::DirectX& dx);
			void UpdateVertexData(Device::DirectX& dx, const void* data, uint size);

			GET_CONST_ACCESSOR(VertexCount, uint, _param.vertexCount);
			GET_CONST_ACCESSOR(Key, const std::string&, _param.key);

			GET_CONST_ACCESSOR(Semantics, const std::vector<Shader::VertexShader::SemanticInfo>&, _semantics);

		private:
			BaseBuffer	_baseBuffer;
			Param		_param;

			std::vector<Shader::VertexShader::SemanticInfo>	_semantics; //attributes
		};
	}
}