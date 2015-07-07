#ifndef OBJWITHPYCALLBACK_H
#define OBJWITHPYCALLBACK_H


#include <iostream>


class Callback;

class ObjWithPyCallback
{
    public:
        ObjWithPyCallback();
        ~ObjWithPyCallback();
        void setCallback(Callback& callback);
        void call();

    private:
        Callback* callback_;
};

#endif
