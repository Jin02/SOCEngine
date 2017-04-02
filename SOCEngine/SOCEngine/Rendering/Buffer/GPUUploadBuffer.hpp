#pragma once

#include "ShaderResourceBuffer.h"
#include "VectorIndexer.hpp"
#include <memory>
#include "DirectX.h"

namespace Rendering
{
	namespace Buffer
	{
		template<typename KeyType, typename T>
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
				uint count = _pool.GetSize();
				const void* raw = _pool.GetVector().data();

				_srBuffer.UpdateResourceUsingMapUnMap(dx, raw, count * sizeof(T));
			}

			// _buffer
			void Add(const KeyType& key, const T& object)	{ _pool.Add(key, object);		}
			void Delete(const KeyType& key)					{ _pool.Delete(key);			}
			void DeleteAll()								{ _pool.DeleteAll();			}
			auto Find(const KeyType& key) const				{ return _pool.Find(key);		}
			auto Find(const KeyType& key)					{ return _pool.Find(key);		}
			bool Has(const KeyType& key) const				{ return _pool.Has(key);		}
			T& Get(uint index)								{ return _pool.Get(index);	}

			GET_CONST_ACCESSOR(Size, const uint, _pool.GetSize());
			GET_CONST_ACCESSOR(ShaderResourceBuffer, const ShaderResourceBuffer&, _srBuffer);

		private:
			Core::VectorHashMap<KeyType, T>			_pool;
			ShaderResourceBuffer					_srBuffer;
		};
	}
}
