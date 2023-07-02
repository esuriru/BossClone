#include "Core/Core.h"
#include "Component.h"
#include <unordered_map>
#include <typeindex>

class GameObject
{
private:
    template<typename T>
    using is_component = std::enable_if_t<std::is_base_of_v<Component, T>>;

public:

    template<typename T, typename = is_component<T>>
    Ref<T> GetComponent()
    {
        auto iterator = components_.find(typeid(T));
        if (iterator != components_.end())
        {
            return static_cast<Ref<T>>(iterator->second);
        }
        return nullptr;
    }

    template<typename T, typename = is_component<T>> 
    Ref<T> AddComponent() 
    {
        return static_cast<Ref<T>>(components_.insert(
            std::make_pair(typeid(T), std::make_shared<T>()))->second);
    }

    template<typename T, typename = is_component<T>>
    void RemoveComponent()
    {
        components_.erase(typeid(T));
    }

private:
    std::unordered_map<std::type_index, Ref<Component>> components_;
     
};
