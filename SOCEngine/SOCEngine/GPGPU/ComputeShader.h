#pragma once

#include "BaseShader.h"
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
				bool IsValid() const { return x && y && z; }

				uint x, y, z;
			};

		public:
			ComputeShader(const DXResource<ID3DBlob>& blob, const std::string& key);

		public:
			void Initialize(Device::DirectX& dx);
			void Dispatch(Device::DirectX& dx);
			
			static void BindTexture(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Texture::BaseTexture> tex);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, const std::weak_ptr<ID3D11SamplerState> samplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const std::weak_ptr<Buffer::ConstBuffer> cb);
			static void BindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Buffer::ShaderResourceBuffer> srBuffer);
			static void BindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind, const std::weak_ptr<View::UnorderedAccessView> uav, const uint* initialCounts = nullptr);

			static void UnBindTexture(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);
			static void UnBindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind);

			GET_SET_ACCESSOR(ThreadGroupInfo, const ThreadGroup&, _threadGroup);
			GET_CONST_ACCESSOR(Key, const std::string&, _base.GetKey());

		private:
			BaseShader								_base;
			DXResource<ID3D11ComputeShader>			_shader;
			ThreadGroup								_threadGroup;
		};
	}
}
