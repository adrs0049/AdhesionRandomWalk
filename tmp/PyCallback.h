#ifndef PYCALLBACK_H
#define PYCALLBACK_H

#include "Event.h"
#include <iostream>

#include<Python.h>

template<class E>
class PyCallback
{
    PyObject * func;
    PyCallback& operator=(const PyCallback&) = delete;

public:
    PyCallback(const PyCallback& o)
        : func(o.func)
    {
        // without this decref it segfaults in python3
        Py_XDECREF(o.func);
        Py_XINCREF(this->func);
    }

    PyCallback(PyObject * _func)
        : func(_func)
    {
        Py_XINCREF(this->func);
        assert(PyCallable_Check(this->func));
    }

    PyCallback()
        : func(nullptr)
    {}

    ~PyCallback()
    {
        Py_XDECREF(this->func);
    }

    void operator() (const E& e)
    {
        if(func == nullptr || Py_None == func || !PyCallable_Check(func))
            return;
        PyObject* args = Py_BuildValue("(s)", s.c_str());
        PyObject* result = PyObject_Call(func, args, 0);
        Py_DECREF(args);
        Py_XDECREF(result);
    }
};

#endif
