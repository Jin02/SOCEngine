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
			ConstBuffer();
			GET_CONST_ACCESSOR(BaseBuffer, const BaseBuffer&, _base);

			void Initialize(Device::DirectX& dx, uint size);
			void UpdateSubResource(Device::DirectX& dx, const void* data);

		private:
			BaseBuffer			_base;
			uint				_size;
		};

		template <typename Type>
		class ExplicitConstBuffer : public ConstBuffer
		{
		public:
			void Initialize(Device::DirectX& dx)
			{
				Initialize(dx, sizeof(Type));
			}

			void UpdateSubResource(Device::DirectX& dx, const Type& data)
			{
				UpdateSubResource(dx, &data);
			}

		private:
			using ConstBuffer::Initialize;
			using ConstBuffer::UpdateSubResource;
		};
	}
}