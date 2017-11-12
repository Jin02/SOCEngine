#pragma once

#include "DirectX.h"
#include "BindIndexInfo.h"

#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"
#include "ConstBuffer.h"

#include "ComputeShader.h"

namespace Rendering
{
	namespace Shader
	{
		template <class ShaderType>
		class AutoBinderSRV final
		{
		public:
			AutoBinderSRV(Device::DirectX& dx, TextureBindIndex bind, const View::ShaderResourceView& srv)
				: _dx(dx), _bind(bind)
			{
				ShaderType::BindShaderResourceView(dx, bind, srv);
			}

			~AutoBinderSRV()
			{
				ShaderType::UnBindShaderResourceView(_dx, _bind);
			}

			DISALLOW_ASSIGN_COPY(AutoBinderSRV<ShaderType>);

		private:
			TextureBindIndex _bind;
			Device::DirectX& _dx;
		};

		// Only Compute Shader
		class AutoBinderUAV final
		{
		public:
			AutoBinderUAV(Device::DirectX& dx, UAVBindIndex bind, const View::UnorderedAccessView& uav)
				: _dx(dx), _bind(bind)
			{
				ComputeShader::BindUnorderedAccessView(dx, bind, uav);
			}
			~AutoBinderUAV()
			{
				ComputeShader::UnBindUnorderedAccessView(_dx, _bind);
			}

			DISALLOW_ASSIGN_COPY(AutoBinderUAV);

		private:
			UAVBindIndex		_bind;
			Device::DirectX&	_dx;
		};

		template <class ShaderType>
		class AutoBinderCB final
		{
		public:
			AutoBinderCB(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb)
				: _dx(dx), _bind(bind)
			{
				ShaderType::BindConstBuffer(dx, bind, cb);
			}
			~AutoBinderCB()
			{
				ShaderType::UnBindConstBuffer(_dx, _bind);
			}

			DISALLOW_ASSIGN_COPY(AutoBinderCB<ShaderType>);

		private:
			ConstBufferBindIndex	_bind;
			Device::DirectX&		_dx;
		};

		template <class ShaderType>
		class AutoBinderSampler final
		{
		public:
			AutoBinderSampler(Device::DirectX& dx, SamplerStateBindIndex bind, RenderState::SamplerState state)
				: _dx(dx), _bind(bind)
			{
				ShaderType::BindSamplerState(dx, bind, state);
			}
			~AutoBinderSampler()
			{
				ShaderType::UnBindSamplerState(_dx, _bind);
			}

			DISALLOW_ASSIGN_COPY(AutoBinderSampler<ShaderType>);

		private:
			SamplerStateBindIndex	_bind;
			Device::DirectX&		_dx;
		};
	}
}