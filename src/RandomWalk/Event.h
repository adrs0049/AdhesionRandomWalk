#pragma once

#include <functional>
#include <vector>
#include <utility>
#include "EventListener.h"
#include <iostream>

class AbstractEvent
{
    public:
        virtual bool removeListener(int id) = 0;
};

template<typename E>
class Event : public AbstractEvent
{
    using value_type = std::vector<std::pair<std::function<void (E)>, int>>;
    using size_type = typename value_type::size_type;

    public:
        EventListener<E> createListener(std::function<void (E)> listener)
        {
            listeners.emplace_back(listener, EventListenerId);
            EventListenerId++;
            return {this, EventListenerId-1};
        }

        ~Event() {}

        void notifyListeners(E e)
        {
            for (auto& l : listeners)
                l.first(e);
        }

        size_type size() const { return listeners.size(); }

        bool removeListener(int id)
        {
            auto iter = listeners.begin();
            while (iter != listeners.end())
            {
                if (iter->second == id)
                {
                    listeners.erase(iter);
                    return true;
                }
                iter++;
            }
            return false;
        }

    protected:
        value_type listeners;
        friend class EventListener<E>;
        int EventListenerId = 0;
};


