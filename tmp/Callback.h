#ifndef CALLBACK_H
#define CALLBACK_H

#include <iostream>

class ObjWithPyCallback;

class Callback
{
    public:
        Callback() {}

        virtual ~Callback() {}
        virtual void call(ObjWithPyCallback& object) {}
};

#endif
