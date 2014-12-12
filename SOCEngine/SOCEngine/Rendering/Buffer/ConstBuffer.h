#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer : public BaseBuffer
		{	
		public:
			enum Usage
			{
				VertexShader,
				PixelShader,
				Staging
			};

		public:
			ConstBuffer();
			~ConstBuffer();

		private:
			bool CreateUsageVSPS(unsigned int size);
			bool CreateUsageStaging(unsigned int size, unsigned int stride);

		public:
			bool Create(unsigned int size, unsigned int stride, Usage usage, const void* sysMem = nullptr);
			void UpdateSubresource(ID3D11DeviceContext* context, const void* data);
		};
	}
}