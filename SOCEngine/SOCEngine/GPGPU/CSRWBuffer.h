#pragma once

#include "UnorderedAccessView.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSRWBuffer : public Rendering::Shader::UnorderedAccessView
		{
		private:
			ID3D11Buffer*				_buffer;
			ID3D11ShaderResourceView*	_srv;

		public:
			CSRWBuffer();
			virtual ~CSRWBuffer();

		public:
			void Initialize(DXGI_FORMAT format, uint stride, uint num);
			void Destroy();

		public:
			GET_ACCESSOR(Buffer, ID3D11Buffer*, _buffer);
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView* const*, &_srv);
		};
	}
}
