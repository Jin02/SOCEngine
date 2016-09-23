#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Buffer
	{
		class RawBuffer : public ShaderResourceBuffer
		{
		public:
			enum class Flag
			{
				ONLY_UAV,
				ONLY_SRV,
				ALL_VIEW
			};

		private:
			View::UnorderedAccessView*		_uav;

		public:
			RawBuffer();
			virtual ~RawBuffer();

		public:
			bool Initialize(uint stride, uint elemNum, Flag flag);
			void Destroy();

		public:
			GET_ACCESSOR(UnorderedAccessView, const View::UnorderedAccessView*, _uav);
		};
	}
}