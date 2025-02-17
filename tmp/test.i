%module test

%{
#include "Event.h"
#include "EventListener.h"
#include <iostream>
#include <vector>

class PyCallback
{
    PyObject * func;
    PyCallback& operator=(const PyCallback&) = delete;

public:
    PyCallback(const PyCallback& o) : func(o.func)
    {
        // without this decref it segfaults in python3
        Py_XDECREF(o.func);
        Py_XINCREF(this->func);
    }

    PyCallback(PyObject * _func) : func(_func)
    {
        Py_XINCREF(this->func);
        assert(PyCallable_Check(this->func));
    }

    PyCallback() : func(nullptr) {}

    ~PyCallback()
    {
        Py_XDECREF(this->func);
    }

    void operator() (const std::string& s)
    {
        if(func == nullptr || Py_None == func || !PyCallable_Check(func))
            return;
        PyObject * args = Py_BuildValue("(s)", s.c_str());
        PyObject* result = PyObject_Call(func, args, 0);
        Py_DECREF(args);
        Py_XDECREF(result);
    }
};

%}

%include "test.h"

%inline %{
    void register_handler(const Event& e, PyObject *callback)
    {
        register_handler(e, PyCallback(callback));
    }

    #include <functional>
    void register_handler(PyObject * callback)
    {   
        auto py_callback = PyCallback(callback);
    }
%}
