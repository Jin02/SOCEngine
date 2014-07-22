#pragma once

template< class T >
class Singleton
{
protected:
	static T* _instance;

protected:
	Singleton(){}
	virtual ~Singleton(){}

private:
	Singleton(const Singleton&){}
	void operator=(Singleton const&){}

public:
	static T* GetInstance()
	{
		if(_instance == nullptr)
			_instance = new T();

		return _instance;
	}

	static void Destroy()
	{
		if(_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	}
};

template< class T > T* Singleton<T>::_instance = nullptr;