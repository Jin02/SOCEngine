#pragma once

#include "Common.h"

namespace Core
{
	template< class T >
	class Singleton
	{
	public:
		DISALLOW_ASSIGN(Singleton);

		static T* SharedInstance()
		{
			if (_instance == nullptr)
				_instance = new T();

			return _instance;
		}

		static void Destroy()
		{
			if (_instance != nullptr)
			{
				delete _instance;
				_instance = nullptr;
			}
		}

	private:
		static T* _instance;
	};

	template< class T > T* Singleton<T>::_instance = nullptr;
}