#pragma once

#include "BaseBuffer.h"
#include <functional>
#include "UnorderedAccessView.h"
#include "ShaderResourceView.h"

namespace Rendering
{
	namespace Buffer
	{
		class RAWBuffer : private BaseBuffer
		{
		private:
			View::ShaderResourceView*		_srv;
			View::UnorderedAccessView*		_uav;

		public:
			RAWBuffer();
			virtual ~RAWBuffer();

		public:
			bool Initialize(uint stride, uint elemNum);
			void Destroy();

		public:
			GET_ACCESSOR(SRV, const View::ShaderResourceView*,	_srv);
			GET_ACCESSOR(UAV, const View::UnorderedAccessView*,	_uav);
		};
	}
}