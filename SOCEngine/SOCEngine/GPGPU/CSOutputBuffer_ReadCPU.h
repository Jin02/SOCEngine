#pragma once

#include "BaseShader.h"
#include "ConstBuffer_CPURead.h"
#include "CSOutputBuffer.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSOutputBuffer_ReadCPU : public CSOutputBuffer
		{
		public:
			CSOutputBuffer_ReadCPU();
			~CSOutputBuffer_ReadCPU();

		public:
			bool Create(unsigned int bufferSize, unsigned int unorderAccessViewSize);
			bool Create(unsigned int stride, unsigned int num, bool useShader);

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}
		};
	}
}
