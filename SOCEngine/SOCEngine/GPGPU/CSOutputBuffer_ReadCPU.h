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
			bool Initialize(unsigned int bufferSize, unsigned int unorderAccessViewSize);
			bool Initialize(unsigned int stride, unsigned int num, bool useShader);

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}
		};
	}
}
