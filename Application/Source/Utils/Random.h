#include <random>

namespace Random
{
    static int Range(int a, int b)
    {
        static std::random_device device;
        static std::mt19937 generator(device());
        return (std::uniform_int_distribution<int>(a, b))(generator);
    }
}
