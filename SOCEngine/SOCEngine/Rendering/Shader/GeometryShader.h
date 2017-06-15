#pragma once

#include "BaseShader.hpp"

namespace Rendering
{
	namespace Shader
	{
		class GeometryShader final
		{
		public:
			GeometryShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key);

			void Initialize(Device::DirectX& dx);
			
			void BindShaderToContext(Device::DirectX& dx);
			void UnBindShaderToContext(Device::DirectX& dx);
			
			static void BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, View::ShaderResourceView& srv);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, ID3D11SamplerState* samplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, Buffer::ConstBuffer& cb);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);

			static constexpr const char* GetCompileCode() { return "gs"; }
			GET_CONST_ACCESSOR(Key, const std::string&, _baseShader.GetKey());

		private:
			DXSharedResource<ID3D11GeometryShader>	_shader;
			BaseShader							_baseShader;
		};
	}
}
