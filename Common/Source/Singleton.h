#pragma once
template<typename T>
class Singleton
{
private:
	static T* _instance;
protected:
	Singleton() {}
	virtual ~Singleton() {}
public:
	static T* GetInstance()
	{
		if (_instance == nullptr)
			_instance = new T();
		return _instance;
	}
	static void Destroy()
	{
		if (_instance != nullptr) {
			delete _instance;
			_instance = nullptr;
		}
	}
	void operator=(const Singleton&) = delete;
	Singleton(const Singleton&) = delete;
};

template <typename T>
T* Singleton<T>::_instance = nullptr;

