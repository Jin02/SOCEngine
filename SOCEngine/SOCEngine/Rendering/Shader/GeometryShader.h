#pragma once

#include "BaseShader.h"

namespace Rendering
{
	namespace Shader
	{
		class GeometryShader final
		{
		public:
			GeometryShader(const DXResource<ID3DBlob>& blob, const std::string& key);

			void Initialize(Device::DirectX& dx);
			
			void BindShaderToContext(Device::DirectX& dx);
			void UnBindShaderToContext(Device::DirectX& dx);
			
			static void BindTexture(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Texture::BaseTexture> tex);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, const std::weak_ptr<ID3D11SamplerState> samplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const std::weak_ptr<Buffer::ConstBuffer> cb);
			static void BindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Buffer::ShaderResourceBuffer> srBuffer);

			static void UnBindTexture(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);
			static void UnBindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind);

			static constexpr const char* GetCompileCode() { return "gs"; }
			GET_CONST_ACCESSOR(Key, const std::string&, _baseShader.GetKey());

		private:
			DXResource<ID3D11GeometryShader>	_shader;
			BaseShader							_baseShader;
		};
	}
}
