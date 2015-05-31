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
			bool Initialize(unsigned int stride, unsigned int num, const void* sysMem = nullptr);

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
		};
	}
}
