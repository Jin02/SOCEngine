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
				ThreadGroup(uint _x, uint _y, uint _z) : x(_x), y(_y), z(_z){}
				bool IsValid() const { return (x != 0) & (y != 0) & (z != 0); }

				uint	x = 0,
						y = 0,
						z = 0;
			};

		public:
			ComputeShader() = default;
			ComputeShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key);

		public:
			void Initialize(Device::DirectX& dx);
			void Dispatch(Device::DirectX& dx);
			
			static void BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, const View::ShaderResourceView& srv);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, RenderState::SamplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb);
			static void BindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind, const View::UnorderedAccessView& uav, const uint* initialCounts = nullptr);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);
			static void UnBindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind);

			GET_CONST_ACCESSOR(ThreadGroupInfo,	const ThreadGroup&,		_threadGroup);
			SET_ACCESSOR(ThreadGroupInfo,		const ThreadGroup&,		_threadGroup);

			GET_CONST_ACCESSOR(Key,				const std::string&,		_base.GetKey());

		private:
			BaseShader								_base;
			DXSharedResource<ID3D11ComputeShader>	_shader;
			ThreadGroup								_threadGroup = ThreadGroup(0, 0, 0);
		};
	}
}
