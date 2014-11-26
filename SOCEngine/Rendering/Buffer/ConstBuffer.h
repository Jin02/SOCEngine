#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer : public BaseBuffer
		{	
		public:
			enum BasicSlot
			{
				Transform = 0,
				MaterialColor
			};

		public:
			ConstBuffer();
			~ConstBuffer();

		public:
			bool Create(unsigned int size);
			void UpdateSubresource(ID3D11DeviceContext* context, const void* data);
		};
	}
}