#ifndef PYCALLBACK_H
#define PYCALLBACK_H

#include <iostream>
#include <vector>
#include <concepts.h>
#include <Python.h>

class PyCallback
{
    PyObject * func = nullptr;
    PyCallback& operator=(const PyCallback&) = delete;

public:
    PyCallback(const PyCallback& o) : func(o.func)
    {
        // without this decref it segfaults in python3
        Py_XDECREF(o.func);
        Py_XINCREF(this->func);
    }

    PyCallback(PyCallback&& other)
    {
        func = other.func;
        other.func = nullptr;

        Py_XDECREF(other.func);
        Py_XINCREF(func);
    }

    PyCallback(PyObject * _func) : func(_func)
    {
        Py_XINCREF(this->func);
        assert(PyCallable_Check(this->func));
    }

    PyCallback() {}

    PyCallback& operator=(PyCallback&& rhs)
    {
        if (this != &rhs)
        {
            func = rhs.func;
            rhs.func = nullptr;
        }
        return *this;
    }

    ~PyCallback()
    {
        Py_XDECREF(this->func);
    }

    void operator() (const std::vector<unsigned int> elements)
    {
        if(func == nullptr || Py_None == func || !PyCallable_Check(func))
            return;
        PyObject* pylist = nullptr;
        PyObject* item = nullptr;
        pylist = PyList_New(elements.size());
        if (pylist != nullptr)
        {
            std::size_t idx {0};
            for (const auto& element : elements)
            {
               item = PyLong_FromLong(element);
               PyList_SET_ITEM(pylist, idx++, item);
            }
        }

        PyObject* args = Py_BuildValue("(O)", pylist);
        PyObject* result = PyObject_CallObject(func, args);

        Py_DECREF(pylist);
        Py_DECREF(args);
        Py_DECREF(item);
        Py_XDECREF(result);
    }
};

#endif
