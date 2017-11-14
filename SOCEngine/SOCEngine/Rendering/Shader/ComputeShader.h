#pragma once

#include "BaseShader.hpp"
#include "ShaderResourceBuffer.h"
#include "Texture2D.h"
#include "BindIndexInfo.h"

namespace Rendering
{
	namespace Shader
	{
		class ComputeShader final
		{
		public:
			struct ThreadGroup
			{
				uint x = 0;
				uint y = 0;
				uint z = 0;
				ThreadGroup(uint _x, uint _y, uint _z) : x(_x), y(_y), z(_z) {}
			};

		public:
			ComputeShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key);

		public:
			void Initialize(Device::DirectX& dx);
			void Dispatch(Device::DirectX& dx, const ComputeShader::ThreadGroup&& group) {Dispatch(dx, group); }
			void Dispatch(Device::DirectX& dx, const ComputeShader::ThreadGroup& group);

			static void BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, const View::ShaderResourceView& srv);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, RenderState::SamplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb);
			static void BindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind, const View::UnorderedAccessView& uav, const uint* initialCounts = nullptr);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);
			static void UnBindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind);

			GET_CONST_ACCESSOR(Key,				const std::string&,		_base.GetKey());

		private:
			BaseShader								_base;
			DXSharedResource<ID3D11ComputeShader>	_shader;
		};
	}
}
