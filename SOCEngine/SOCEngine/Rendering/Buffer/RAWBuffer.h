#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Buffer
	{
		class RAWBuffer : public ShaderResourceBuffer
		{
		private:
			View::UnorderedAccessView*		_uav;

		public:
			RAWBuffer();
			virtual ~RAWBuffer();

		public:
			bool Initialize(uint stride, uint elemNum);
			void Destroy();

		public:
			GET_ACCESSOR(UnorderedAccessView, const View::UnorderedAccessView*,	_uav);
		};
	}
}
