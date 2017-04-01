#pragma once

#include "BaseShader.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader final
		{
		public:
			PixelShader(const DXResource<ID3DBlob>& blob, const std::string& key);

			void Initialize(Device::DirectX& dx);
			
			void BindShaderToContext(Device::DirectX& dx);
			void UnBindShaderToContext(Device::DirectX& dx);
			
			static void BindTexture(Device::DirectX& dx,					TextureBindIndex bind,		const Texture::TextureGPUView& tex);
			static void BindSamplerState(Device::DirectX& dx,				SamplerStateBindIndex bind,	DXResource<ID3D11SamplerState>& samplerState);
			static void BindConstBuffer(Device::DirectX& dx,				ConstBufferBindIndex bind,	const Buffer::ConstBuffer& cb);
			static void BindShaderResourceBuffer(Device::DirectX& dx,		TextureBindIndex bind,		const Buffer::ShaderResourceBuffer& srBuffer);

			static void UnBindTexture(Device::DirectX& dx,				TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx,			SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx,			ConstBufferBindIndex bind);
			static void UnBindShaderResourceBuffer(Device::DirectX& dx,	TextureBindIndex bind);

			static constexpr const char* GetCompileCode() { return "ps"; }
			GET_CONST_ACCESSOR(Key, const std::string&, _baseShader.GetKey());

		private:
			BaseShader						_baseShader;
			DXResource<ID3D11PixelShader>	_shader;
		};
	}
}
