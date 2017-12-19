#pragma once

#include "BaseShader.hpp"

namespace Rendering
{
	namespace Shader
	{
		class GeometryShader final
		{
		public:
			GeometryShader() = default;
			GeometryShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key);

			void Initialize(Device::DirectX& dx);
			void Destroy();
			
			void BindShaderToContext(Device::DirectX& dx) const;
			static void UnBindShaderToContext(Device::DirectX& dx);
			
			static void BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, const View::ShaderResourceView& srv);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, RenderState::SamplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);

			static constexpr const char* GetCompileCode() { return "gs"; }
			GET_CONST_ACCESSOR(Key,			const std::string&,		_baseShader.GetKey());
			GET_CONST_ACCESSOR(IsCanUse,	bool,					_baseShader.GetIsCanUse());

		private:
			DXSharedResource<ID3D11GeometryShader>	_shader;
			BaseShader								_baseShader;
		};
	}
}
