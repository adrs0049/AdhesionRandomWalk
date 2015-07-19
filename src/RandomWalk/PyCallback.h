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
        // Py_XDECREF(o.func);
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

	void operator() (SimulationData data)
    {
        if(func == nullptr || Py_None == func) // || !PyCallable_Check(func))
            return;

		if(!data.states.size())
		{
			std::cerr << "data states is empty!" << std::endl;
			return;
		}

		std::size_t length = data.states.size();
        PyObject* pylist = nullptr;
		PyObject* item = nullptr;
		pylist = PyList_New(length);
		//Py_INCREF(pylist);
        if (pylist != nullptr)
        {
			//PyObject* item = nullptr;

			std::cout << "size=" << length << std::endl;
			std::cout << "state=(";
			for (std::size_t idx = 0; idx < length; idx++)
            {
				unsigned long temp = data.states[idx];
				std::cout << "[" << idx << "]="<< temp << ", ";
				item = PyLong_FromLong(temp);
				if (item == nullptr){
					std::cerr << "ERROR item is null!!" << std::endl;
				}
				PyList_SET_ITEM(pylist, idx, item);

            }
			std::cout << ")." << std::endl;
			Py_XDECREF(item);
        }
		else
		{
			std::cerr << "PyList is NULL" << std::endl;
			return;
		}

		PyObject* args = PyTuple_New(0);
		PyObject* kwargs = Py_BuildValue("{s:O, s:l, s:d}",
				"states", pylist, "steps", data.steps, "time", data.time);

		Py_INCREF(args);
		Py_INCREF(kwargs);

		std::cerr << "steps=" << data.steps << " data.sz="
			<< data.states.size() << " time=" << data.time << "." << std::endl;

		assert(args!=nullptr);
		assert(kwargs!=nullptr);

		std::cerr << "CALL NOW!" << std::endl;
		PyObject* myobject_method = PyObject_GetAttrString(func, "callback");
		if (!PyCallable_Check(myobject_method))
		{
			std::cerr << "it's not callable" << std::endl;
			Py_XDECREF(args);
			Py_XDECREF(kwargs);
			Py_XDECREF(myobject_method);

			return;
		}
		PyObject* ret = PyObject_Call(myobject_method, args, kwargs);

		Py_XDECREF(pylist);
		Py_XDECREF(args);
		Py_XDECREF(kwargs);
		Py_XDECREF(myobject_method);

		std::cerr << "CALLED" << std::endl;

		if (ret==nullptr) {
			std::cerr << "ERROR in call" << std::endl;
		}

		Py_XDECREF(ret);
    }
};

#endif
