#include <functional>
#include <map>
#include <vector>

#include "Utils/Singleton.h"
#include "Event.h"

class EventManager : Utility::Singleton<EventManager>
{
public:
    EventManager() = default;

    using EventCallback = std::function<void(Event&)>;

public:
    inline void Subscribe(const Event& e, const EventCallback& callback)
    {
        observers_[e.GetEventType()].emplace_back(callback);
    }

    inline void Invoke(Event& e)
    {
        auto type = e.GetEventType(); 
        if (observers_.find(type) == observers_.end())
        {
            return;
        }

        auto& observers = observers_.at(type);

        for (auto&& observer : observers)
        {
            observer(e);
        }
    } 
    
private:
    std::map<EventType, std::vector<EventCallback>> observers_;

};