#pragma once

#include "ShaderResourceBuffer.h"
#include "VectorIndexer.hpp"
#include <memory>
#include "DirectX.h"

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
				const void* raw = _pool.GetVector().data();
				_srBuffer.UpdateResourceUsingMapUnMap(dx, raw, _pool.GetSize() * sizeof(T));
			}

			// _buffer
			inline void AddData(uint key, T& data)		{	_pool.Add(key, data);			}
			inline void AddData(uint key, T&& data)		{	_pool.Add(key, data);			}
			inline void SetData(uint key, T& data)		{	(*_pool.Find(key)) = data;		}
			inline void SetData(uint key, T&& data)		{	(*_pool.Find(key)) = data;		}
			inline void Delete(uint key)				{	_pool.Delete(key);				}
			inline void DeleteAll()						{	_pool.DeleteAll();				}
//			inline T& Get(uint index)					{	return _pool[index];			}
			inline uint GetSize() const					{	return _pool.GetSize();			}

			GET_ACCESSOR(ShaderResourceBuffer,	auto&, _srBuffer);
			GET_CONST_ACCESSOR(IndexBook,		const auto&, _pool.GetIndexer());

		private:
			Core::VectorMap<uint, T>	_pool;
			ShaderResourceBuffer		_srBuffer;
		};
	}
}
