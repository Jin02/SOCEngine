#pragma once

#include "BaseShader.h"
#include "CPUReadConstBuffer.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSOutputBuffer : public Rendering::Buffer::BaseBuffer
		{
		private:
			ID3D11UnorderedAccessView	*_uav;

		public:
			CSOutputBuffer();
			~CSOutputBuffer();

		public:
			bool Create(unsigned int stride, unsigned int num);

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}

		public:
			GET_ACCESSOR(UnorderedAccessView, ID3D11UnorderedAccessView*, _uav);
		};
	}
}
