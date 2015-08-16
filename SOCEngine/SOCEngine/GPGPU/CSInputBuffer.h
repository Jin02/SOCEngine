#pragma once

#include "BaseShader.h"
#include "BaseBuffer.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSInputBuffer : public Rendering::Buffer::BaseBuffer
		{
		private:
			ID3D11ShaderResourceView*	_srv;

		public:
			CSInputBuffer();
			virtual ~CSInputBuffer();

		public:
			void Initialize(uint stride, uint num, DXGI_FORMAT format, const void* sysMem);

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
		};
	}
}
