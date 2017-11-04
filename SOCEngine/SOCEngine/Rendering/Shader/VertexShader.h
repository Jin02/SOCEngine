#pragma once

#include "BaseShader.hpp"
#include "ConstBuffer.h"
#include "BindIndexInfo.h"

namespace Rendering
{
	namespace Shader
	{
		class VertexShader final
		{
		public:
			struct SemanticInfo
			{
				std::string	name			= "";
				uint		semanticIndex	= -1;
				uint		size			= 0;
			};

			VertexShader() = default;
			VertexShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key);

			void Initialize(Device::DirectX& dx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations);

			void BindShaderToContext(Device::DirectX& dx) const;
			void BindInputLayoutToContext(Device::DirectX& dx) const;
			void UnBindShaderToContext(Device::DirectX& dx) const;
			void UnBindInputLayoutToContext(Device::DirectX& dx) const;

			static void BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, View::ShaderResourceView& view);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, ID3D11SamplerState* const samplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, Buffer::ConstBuffer& cb);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);

			static constexpr const char* GetCompileCode() { return "vs"; }

			GET_CONST_ACCESSOR(SemanticInfos,	const std::vector<SemanticInfo>&,	_semanticInfo);
			GET_CONST_ACCESSOR(Key,				const std::string&,					_baseShader.GetKey());
			GET_CONST_ACCESSOR(IsCanUse,		bool,								_baseShader.GetIsCanUse());

		private:
			BaseShader								_baseShader;
			DXSharedResource<ID3D11VertexShader>	_shader;
			DXSharedResource<ID3D11InputLayout>		_layout;

			std::vector<SemanticInfo>				_semanticInfo;
		};
	}
}
