#include "EventListener.h"
#include "Event.h"

AbstractEventListener::AbstractEventListener(AbstractEvent *ae, int listenerId)
    : listenerId(listenerId), ae(ae) {}

AbstractEventListener::~AbstractEventListener()
{
    destroyListener();
}

void AbstractEventListener::destroyListener()
{
    if(ae != nullptr)
        ae->removeListener(listenerId);
}
