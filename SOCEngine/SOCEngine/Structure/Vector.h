#pragma once

#include <vector>
#include "BaseStructure.h"
#include <functional>

namespace Structure
{
	template <typename Key, class Object>
	class Vector : public BaseStructure<Key, Object>
	{
	public:
		typedef std::pair<Key, BaseStructure<Key, Object>::Data>	Type;

	protected:
		std::vector<Type> _vector;

	public:
		Vector(void){}
		virtual ~Vector(void)
		{
			DeleteAll(true);
		}

	private:
		static void PackingType(Type& out, const Key& key, Object* object, bool copy)
		{
			Data data;
			data.first = copy;
			data.second = copy ? new Object((*object)) : object;

			out.first = key;
			out.second = data;
		}

	public:
		virtual Object* Add(const Key& key, Object* object, bool copy = false)
		{
			Type type;
			PackingType(type, key, object, copy);

			_vector.push_back(type);
			return object;
		}

		virtual Object* Find(const Key& key)
		{
			for(std::vector<Type>::iterator iter = _vector.begin(); iter != _vector.end(); ++iter)
			{
				if(GET_KEY_FROM_ITERATOR(iter) == key)
					return GET_CONTENT_FROM_ITERATOR(iter);
			}

			return nullptr;
		}

		Object* Get(unsigned int index)
		{
			if(index > _vector.size())
				return nullptr;

			return _vector[index];
		}

		virtual void Delete(const Key& key, bool contentRemove = false)
		{
			for(std::vector<Type>::iterator iter = _vector.begin(); iter != _vector.end(); ++iter)
			{
				if(GET_KEY_FROM_ITERATOR(iter) == key)
				{
					if(contentRemove)
						SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );

					_vector.erase(iter);
					return;
				}
			}
		}

		void DeleteAll(bool contentRemove = false)
		{
			if(contentRemove)
			{
				for(std::vector<Type>::iterator iter = _vector.begin(); iter != _vector.end(); ++iter)
				{
					if( GET_IS_COPY_FROM_ITERATOR(iter) )
						SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );
				}
			}

			_vector.clear();
		}

		
		void Iterate(const std::function<void(Object* mesh)>& recvFunc)
		{
			for(std::vector<Type>::iterator iter = _vector.begin(); iter != _vector.end(); ++iter)
				recvFunc(GET_CONTENT_FROM_ITERATOR(iter));
		}

		GET_ACCESSOR(Vector, const std::vector<Type>&, _vector);
		GET_ACCESSOR(Size, unsigned int, _vector.size());
	};
}