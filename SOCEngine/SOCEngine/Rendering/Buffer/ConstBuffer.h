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
			ConstBuffer(Device::DirectX& dx, uint size) { Initialize(dx, size); }

			GET_ALL_ACCESSOR(BaseBuffer,	BaseBuffer&,	_base);
//			GET_ACCESSOR(Raw,				auto,			_base.GetRaw());

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
			ExplicitConstBuffer(Device::DirectX& dx) { Initialize(dx); }

			void Initialize(Device::DirectX& dx)							{ _cb.Initialize(dx, sizeof(Type)); }
			void UpdateSubResource(Device::DirectX& dx, const Type& data)	{ _cb.UpdateSubResource(dx, &data); }

			operator ConstBuffer&()				{ return _cb; }
			operator const ConstBuffer&() const { return _cb; }

		private:
			ConstBuffer _cb;
		};
	}
}
