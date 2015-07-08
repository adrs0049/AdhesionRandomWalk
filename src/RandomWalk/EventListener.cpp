#include "EventListener.h"
#include "Event.h"

AbstractEventListener::AbstractEventListener(AbstractEvent *ae, int listenerId)
    : ae(ae), listenerId(listenerId) {}

AbstractEventListener::~AbstractEventListener()
{
    destroyListener();
}

void AbstractEventListener::destroyListener()
{
    if(ae)
        ae->removeListener(listenerId);
}
