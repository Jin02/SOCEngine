#pragma once

#include "BaseShader.hpp"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader final
		{
		public:
			PixelShader() = default;
			PixelShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key);

			void Initialize(Device::DirectX& dx);
			
			void BindShaderToContext(Device::DirectX& dx) const;
			static void UnBindShaderToContext(Device::DirectX& dx);
			
			static void BindShaderResourceView(Device::DirectX& dx,	TextureBindIndex bind, const View::ShaderResourceView& view);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, RenderState::SamplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);

			static constexpr const char* GetCompileCode() { return "ps"; }

			GET_CONST_ACCESSOR(Key,			const std::string&,	_baseShader.GetKey());
			GET_CONST_ACCESSOR(IsCanUse,	bool,				_baseShader.GetIsCanUse());

		private:
			BaseShader _baseShader;
			DXSharedResource<ID3D11PixelShader> _shader;
		};
	}
}
