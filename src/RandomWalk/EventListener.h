
#pragma once

#include <iostream>

class AbstractEvent;

class AbstractEventListener
{
    public:
        AbstractEventListener() {};
        AbstractEventListener(AbstractEvent *ae, int listenerId);
        AbstractEventListener(const AbstractEventListener&) = delete;
        AbstractEventListener& operator=(const AbstractEventListener& rhs) = delete;

        virtual ~AbstractEventListener();
        friend class AbstractEvent;

    protected:
        void destroyListener();
        int listenerId = -1;
        AbstractEvent *ae = nullptr;
};

template<typename E>
class Event;

template<typename E>
class EventListener : public AbstractEventListener
{
    public:
        EventListener() {}
        EventListener(Event<E>* e, int listenerId) : AbstractEventListener(e, listenerId) {}
        EventListener(const EventListener&) = delete;
        EventListener& operator=(const EventListener& rhs) = delete;

        EventListener(EventListener&& other)
        {
            listenerId = other.listenerId;
            ae = other.ae;
            other.ae = nullptr;
        }

        EventListener& operator=(EventListener&& rhs)
        {
            if (this != &rhs)
            {
                destroyListener();
                listenerId = rhs.listenerId;
                ae = rhs.ae;
                rhs.ae = nullptr;
            }
            return *this;
        }

        friend class Event<E>;
};

//
// updateFunction = [&](E e) { syncValue.value = e; };
//
template<typename E>
class SyncValue : public AbstractEventListener
{
    public:
        SyncValue() {}
        E& getValue()
        {
            return value;
        }

        SyncValue(const SyncValue&) = delete;
        SyncValue& operator=(const SyncValue& rhs) = delete;

        SyncValue& operator=(SyncValue&& rhs)
        {
            if (this != &rhs)
            {
                destroyListener();
                listenerId = rhs.listenerId;
                ae = rhs.ae;
                value = std::move(rhs.value);
                rhs.ae = nullptr;
            }
            return *this;
        }

        SyncValue(SyncValue&& other)
        {
            listenerId = other.listenerId;
            ae = other.ae;
            value = std::move(other.value);
            other.ae = nullptr;
        }

        friend class Event<E>;

    private:
        E value;
};



