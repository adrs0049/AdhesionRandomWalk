#pragma once

#include <map>
#include <memory>
#include <iterator>
#include "Event.h"

enum class EventType { new_data, nothing };

class EventRegistry
{
    using value_type = std::map<EventType, std::shared_ptr<AbstractEvent>>;
    using ptr_type = std::shared_ptr<AbstractEvent>;

public:
    EventRegistry() {};

    EventRegistry(const EventRegistry& other) = delete;
    EventRegistry(EventRegistry&& other) = delete;
    EventRegistry& operator=(const EventRegistry& rhs) = delete;
    EventRegistry& operator=(EventRegistry&& rhs) = delete;

    void register_handler(EventType&& e, ptr_type&& l)
    {
        table.emplace(e, l);
    }

    ptr_type& get_handler(EventType&& e)
    {
        return table[e];
    }

private:
    value_type table;
};

