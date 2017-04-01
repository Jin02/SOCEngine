#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Buffer
	{
		class RawBuffer final : public ShaderResourceBuffer
		{
		public:
			enum class Flag
			{
				ONLY_UAV,
				ONLY_SRV,
				ALL_VIEW
			};

			RawBuffer();
			~RawBuffer();

			void Initialize(Device::DirectX& dx, uint stride, uint elemNum, Flag flag);
			void Destroy();
			
			GET_CONST_ACCESSOR(UnorderedAccessView, const View::UnorderedAccessView&, _uav);

		private:
			View::UnorderedAccessView		_uav;
		};
	}
}