#pragma once

#include "BaseBuffer.h"
#include <memory>
#include <assert.h>

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer
		{
		public:
			ConstBuffer() = default;
			GET_CONST_ACCESSOR(BaseBuffer, const BaseBuffer&, _base);

			void Initialize(Device::DirectX& dx, uint size);
			void UpdateSubResource(Device::DirectX& dx, const void* data);

		private:
			BaseBuffer			_base;
			uint				_size = 0;
		};

		template <typename Type>
		class ExplicitConstBuffer 
		{
		public:
			ExplicitConstBuffer() = default;
			
			void Initialize(Device::DirectX& dx)				{ _cb.Initialize(dx, sizeof(Type)); }
			void UpdateSubResource(Device::DirectX& dx, const Type& data)	{ _cb.UpdateSubResource(dx, &data); }

		private:
			ConstBuffer _cb;
		};
	}
}
