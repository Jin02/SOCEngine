#pragma once

#include "ShaderResourceBuffer.h"
#include "VectorHashMap.h"

namespace Rendering
{
	namespace Buffer
	{
		template<typename KeyType, typename T>
		class GPUUploadBuffer
		{
		private:
			Structure::VectorHashMap<KeyType, T>	_buffer;
			ShaderResourceBuffer*					_srBuffer;

		public:
			GPUUploadBuffer() : _srBuffer(nullptr)
			{
			}

			~GPUUploadBuffer()
			{
				Destory();
			}

		public:
			void Destory()
			{
				SAFE_DELETE(_srBuffer);
				_buffer.DeleteAll();
			}
			void Initialize(uint count, DXGI_FORMAT format, const void* dummy = nullptr)
			{
				Destory();

				_srBuffer = new ShaderResourceBuffer;
				_srBuffer->Initialize(sizeof(T), count, format, dummy, true, 0, D3D11_USAGE_DYNAMIC);
			}

		public: // _buffer
			void Add(const KeyType& key, const T& object)	{	_buffer.Add(key, object);	}
			void Delete(const KeyType& key)					{	_buffer.Delete(key);		}
			void DeleteAll()								{	_buffer.DeleteAll();		}
			const T* Find(const KeyType& key) const			{	return _buffer.Find(key);	}
			bool Has(const KeyType& key) const				{	return _buffer.Has(key);	}
			T& Get(uint index)								{	return _buffer.Get(index);	}
			
			GET_ACCESSOR(Size, const uint, _buffer.GetSize());

		public:
			GET_ACCESSOR(ShaderResourceBuffer, const ShaderResourceBuffer*, _srBuffer);
		};
	}
}