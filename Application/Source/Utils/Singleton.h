#pragma once

namespace Utility
{
    template<typename T>
    class Singleton
    {
    protected:
        Singleton() {}
        virtual ~Singleton() {}

    public:
        static T* Instance()
        {
            static T instance;
            return &instance;
        }

        // We don't want people to copy the Singleton.
        void operator=(const Singleton&) = delete;
        Singleton(const Singleton&) = delete;
    };
}

