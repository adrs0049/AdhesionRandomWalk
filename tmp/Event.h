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
    public:
        EventListener<E> createListener(std::function<void (E)> listener)
        {
            listeners.emplace_back(listener, EventListenerId);
            EventListenerId++;
            return EventListener<E>(this, EventListenerId-1);
        }

        void registerSyncValue(SyncValue<E>& syncValue)
        {
            if (syncValue.listenerId > -1)
                removeListener(syncValue.listenerId);

            std::function<void (E)> updateFunction = [&](E e)
            {
                syncValue.value = e;
            };

            listeners.emplace_back(updateFunction, EventListenerId);
            syncValue.listenerId = EventListenerId;
            syncValue.ae = this;
            EventListenerId++;
        }

        void notifyListeners(E e)
        {
            for (auto& l : listeners)
                l.first(e);
        }

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
        std::vector<std::pair<std::function<void (E)>, int>> listeners;
        friend class EventListener<E>;
        int EventListenerId = 0;
};


