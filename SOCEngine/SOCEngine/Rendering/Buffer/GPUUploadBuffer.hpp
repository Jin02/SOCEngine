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

		public:
			void Initialize(Device::DirectX& dx, uint count, DXGI_FORMAT format, const void* dummy = nullptr)
			{
				Destroy();

				_srBuffer.Destroy();
				_srBuffer.Initialize(dx, sizeof(T), count, format, dummy, false, 0, D3D11_USAGE_DYNAMIC);
			}

			void Destroy()
			{
				_srBuffer.Destroy();
				_base.DeleteAll();
			}

			// _srBuffer
			void UpdateSRBuffer(Device::DirectX& dx)
			{
				uint count = _base.GetSize();
				const void* raw = _base.GetVector().data();

				_srBuffer.UpdateResourceUsingMapUnMap(dx, raw, count * sizeof(T));
			}

			// _buffer
			void Add(const KeyType& key, const T& object)	{ _base.Add(key, object);		}
			void Delete(const KeyType& key)					{ _base.Delete(key);			}
			void DeleteAll()								{ _base.DeleteAll();			}
			auto Find(const KeyType& key) const				{ return _base.Find(key);		}
			auto Find(const KeyType& key)					{ return _base.Find(key);		}
			bool Has(const KeyType& key) const				{ return _base.Has(key);		}
			T& Get(uint index)								{ return _base.Get(index);	}

			GET_CONST_ACCESSOR(Size, const uint, _base.GetSize());
			GET_CONST_ACCESSOR(ShaderResourceBuffer, const ShaderResourceBuffer&, _srBuffer);

		private:
			Core::VectorHashMap<KeyType, T>			_base;
			ShaderResourceBuffer					_srBuffer;
		};
	}
}