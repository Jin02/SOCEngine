#pragma once

#include "ShaderResourceBuffer.h"
#include "VectorIndexer.hpp"
#include <memory>
#include "DirectX.h"
#include <assert.h>

namespace Rendering
{
	namespace Buffer
	{
		template<typename T>
		class GPUUploadBuffer
		{
		public:
			GPUUploadBuffer() = default;

			void Initialize(Device::DirectX& dx, uint count, DXGI_FORMAT format, const void* dummy = nullptr)
			{
				_srBuffer.Initialize(dx, sizeof(T), count, format, dummy, false, 0, D3D11_USAGE_DYNAMIC);
			}

			// _srBuffer
			void UpdateSRBuffer(Device::DirectX& dx)
			{
				const void* raw = _pool.data();
				_srBuffer.UpdateResourceUsingMapUnMap(dx, raw, _pool.size() * sizeof(T));
			}

			// _buffer
			inline void PushData(T& data)				{	_pool.push_back(data);			}
			inline void PushData(T&& data)				{	_pool.push_back(data);			}
			inline void DeleteAll()						{	_pool.clear();					}
			inline uint GetSize() const					{	return _pool.size();			}
			inline T& operator[](uint index)
			{
				assert( (0 <= index) & (index < _pool.size()));
				return _pool[index];
			}

			inline void Delete(uint index)
			{
				auto iter = _pool.begin() + index;
				_pool.erase(iter);
			}

			GET_ACCESSOR(ShaderResourceBuffer, auto&, _srBuffer);

		private:
			std::vector<T>				_pool;
			ShaderResourceBuffer		_srBuffer;
		};
	}
}
