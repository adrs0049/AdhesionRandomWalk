#ifndef PYCALLBACK_H
#define PYCALLBACK_H

#include "Event.h"
#include "EventListener.h"

#include <iostream>
#include<Python.h>

// E = PyObject*

template<typename E>
class PyCallback : public AbstractEventListener
{
public:
    PyCallback() {}

    // STATIC_ASSERT that E is of type pointer

    /*
    PyCallback(const PyCallback& o)
        : func(o.func)
    {
        // without this decref it segfaults in python3
        Py_XDECREF(o.func);
        Py_XINCREF(this->func);
    }
    */

    PyCallback(Event<E>* e, int listenerId)
        : AbstractEventListener(e, listenerId)
    {
        Py_XINCREF(this->func);
        assert(PyCallable_Check(this->func));
    }

    PyCallback& operator=(const PyCallback&) = delete;
    PyCallback(const PyCallback& other) = delete;

    PyCallback(PyCallback&& other)
    {
        listenerId = other.listenerId;
        ae = other.ae;
        func = other.func;

        // python required functions
        Py_XDECREF(other.func);
        Py_XINCREF(this->func);

        // unset everything in other
        other.func = nullptr;
        other.ae = nullptr;
    }

    PyCallback& operator=(PyCallback&& rhs)
    {
        if (this != &rhs)
        {
            destroyListener();
            listenerId = rhs.listenerId;
            ae = rhs.ae;
            func = rhs.func;

            Py_XDECREF(rhs.func);
            Py_XINCREF(this->func);

            rhs.ae = nullptr;
            rhs.func = nullptr;
        }
        return *this;
    }

    ~PyCallback()
    {
        Py_XDECREF(this->func);
    }

    // the call function
    void operator() (const std::string& s)
    {
        if(func == nullptr || Py_None == func || !PyCallable_Check(func))
            return;
        PyObject* args = Py_BuildValue("(s)", s.c_str());
        PyObject* result = PyObject_Call(func, args, 0);
        Py_DECREF(args);
        Py_XDECREF(result);
    }

private:
    E func = nullptr;
};

#endif
