#pragma once

#include "ShaderResourceBuffer.h"
#include "VectorHashMap.h"
#include <memory>

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
			static GPUUploadBuffer* Create(uint count, DXGI_FORMAT format, const void* dummy = nullptr)
			{
				GPUUploadBuffer<KeyType, T>* buffer = new GPUUploadBuffer<KeyType, T>;
				buffer->Initialize(count, format, dummy);
				
				return buffer;
			}
			
			void Initialize(uint count, DXGI_FORMAT format, const void* dummy = nullptr)
			{
				Destory();

				_srBuffer = new ShaderResourceBuffer;
				_srBuffer->Initialize(sizeof(T), count, format, dummy, true, 0, D3D11_USAGE_DYNAMIC);
			}

			void Destory()
			{
				SAFE_DELETE(_srBuffer);
				_buffer.DeleteAll();
			}
			
		public: // _srBuffer
			void UpdateSRBuffer(ID3D11DeviceContext* context)
			{
				uint count		= _buffer.GetSize();
				const void* raw	= _buffer.GetVector().data();

				_srBuffer->UpdateResourceUsingMapUnMap(context, raw, count * sizeof(T));
			}
			
		public: // _buffer
			void Add(const KeyType& key, const T& object)	{	_buffer.Add(key, object);	}
			void Delete(const KeyType& key)					{	_buffer.Delete(key);		}
			void DeleteAll()								{	_buffer.DeleteAll();		}
			const T* Find(const KeyType& key) const			{	return _buffer.Find(key);	}
			T* Find(const KeyType& key)						{	return _buffer.Find(key);	}
			bool Has(const KeyType& key) const				{	return _buffer.Has(key);	}
			T& Get(uint index)								{	return _buffer.Get(index);	}
			
			GET_ACCESSOR(Size, const uint, _buffer.GetSize());

		public:
			GET_ACCESSOR(ShaderResourceBuffer, const ShaderResourceBuffer*, _srBuffer);
		};
	}
}
