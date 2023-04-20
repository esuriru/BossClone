#pragma once

/// <summary>
/// Singleton base class
/// </summary>
/// <typeparam name="T">Class to become a singleton</typeparam>
namespace Nare
{
	namespace Utility
	{
		template<typename T>
		class Singleton
		{
		private:
			static T* instance_;
		protected:
			Singleton() {}
			virtual ~Singleton() {}
		public:
			static T* Instance()
			{
				if (!instance_)
					instance_ = new T();
				return instance_;
			}
			static void Destroy()
			{
				if (instance_) {
					delete instance_;
					instance_ = nullptr;
				}
			}

			// We don't want people to copy the Singleton.
			void operator=(const Singleton&) = delete;
			Singleton(const Singleton&) = delete;
		};

		template <typename T>
		T* Singleton<T>::instance_ = nullptr;
	}
}

