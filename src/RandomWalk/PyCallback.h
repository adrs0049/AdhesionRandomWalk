#ifndef PYCALLBACK_H
#define PYCALLBACK_H

#include <iostream>
#include <vector>
#include <concepts.h>
#include <Python.h>

#include "SimulationData.h"

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

	void operator() (SimulationData&& data)
    {
        if(func == nullptr || Py_None == func || !PyCallable_Check(func))
            return;
        PyObject* pylist = nullptr;
        PyObject* item = nullptr;
        pylist = PyList_New(data.states.size());
        if (pylist != nullptr)
        {
            std::size_t idx {0};
            for (const auto& state : data.states)
            {
               item = PyLong_FromLong(state);
               PyList_SET_ITEM(pylist, idx++, item);
            }
        }

		PyObject* args = PyTuple_New(0);
		PyObject* kwargs = Py_BuildValue("{s:O, s:l, s:d}",
				"states", pylist, "steps", data.steps, "time", data.time);

		PyObject_Call(func, args, kwargs);

		Py_DECREF(kwargs);
        Py_DECREF(pylist);
        Py_DECREF(args);
        Py_DECREF(item);
    }
};

#endif
