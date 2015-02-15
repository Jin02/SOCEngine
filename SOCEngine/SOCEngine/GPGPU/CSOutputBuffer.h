#pragma once

#include "BaseShader.h"
#include "ConstBuffer_CPURead.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSOutputBuffer : public Rendering::Buffer::BaseBuffer
		{
		protected:
			ID3D11UnorderedAccessView	*_uav;

		public:
			CSOutputBuffer();
			~CSOutputBuffer();

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}

		public:
			GET_ACCESSOR(UnorderedAccessView, ID3D11UnorderedAccessView*, _uav);
		};
	}
}
