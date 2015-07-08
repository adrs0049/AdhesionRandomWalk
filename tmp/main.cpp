#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "Event.h"
#include "EventListener.h"

using namespace std;

class Foo
{
    public:
        Foo() {}
        Event<int> event;
        void setValue(int i)
        {
            value = i;
            event.notifyListeners(i);
        }
    private:
        int value;
};

class Moo
{
    public:
        Moo(Foo *f)
        {
            f->event.registerSyncValue(syncVal);
            eventListener = f->event.createListener([](int v) {
                    cout << "Value is "<<v+1<<endl;
                    });
        }
        SyncValue<int> syncVal;
    private:
        EventListener<int> eventListener;
};

int main(int argc, const char * argv[])
{

    Foo f;
    {
        Moo m(&f);
        f.setValue(123);
        cout << "synched value " << m.syncVal.getValue() << endl;
        f.setValue(12);
        cout << "synched value " << m.syncVal.getValue() << endl;
    }
    f.setValue(1234);

return 0;
}
