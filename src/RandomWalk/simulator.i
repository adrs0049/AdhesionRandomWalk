// simulator.i - SWIG interface
%module simulator

%include "exception.i"

%exception {
    try {
        $action
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

%include boundary.i
%include "typemaps.i"
%include <std_vector.i>
%include <std_string.i>
%include <std_shared_ptr.i>

%{
#include "PyCallback.h"
%}

%insert("python") %{
import numpy as np
%}

%inline %{
    #include <algorithm>

    template<typename Container_T>
    void copy_to_buffer( const Container_T& field,
                         typename Container_T::value_type* buffer,
                         typename Container_T::size_type length)
    {
        if (length == field.size())
        {
            std::copy(field.begin(), field.end(), buffer);
        }
    }

%}

%define TYPEMAP_COPY_TO_BUFFER(CLASS...)

%apply (CLASS::value_type* buffer, CLASS::size_type length) {(CLASS::value_type* buffer, CLASS::size_type length)};

%typemap(in) (CLASS::value_type* buffer, CLASS::size_type length)
(int res = 0, Py_ssize_t size_ = 0, void *buffer_ = 0)
{
    res = PyObject_AsWriteBuffer($input, &buffer_, &size_);
    if (res < 0)
    {
        PyErr_Clear();
        %argument_fail(res, "(CLASS::value_type*, CLASS::size_type length)",
            $symname, $argnum);
    }
    $1 = ($1_ltype) buffer_;
    $2 = ($2_ltype) (size_/sizeof($*1_type));
}
%enddef

%define ADD_NUMPY_ARRAY_INTERFACE(PYVALUE, PYCLASS, CLASS...)

TYPEMAP_COPY_TO_BUFFER(CLASS)

%template(_copy_to_buffer_ ## PYCLASS) copy_to_buffer< CLASS >;

%extend CLASS {
%insert("python") %{
def __array__(self):
    """ Enable access to this data as a numpy array """
    a = np.ndarray( shape=( len(self), ), dtype=PYVALUE )
    _copy_to_buffer_ ## PYCLASS(self, a)
    return a
%}
}

%enddef

%{
#define SWIG_FILE_WITH_INIT
#include<vector>
%}

%include "numpy.i"

%init %{
import_array();
%}

%extend std::vector<double> {
    std::vector<double>*
    new_std_vector_double(double* items, int length)
    {
        return new std::vector<double>{items, items + length};
    }
}



%template (DVector) std::vector<double>;
ADD_NUMPY_ARRAY_INTERFACE(float, DVector, std::vector<double>);

%apply (double* IN_ARRAY1, int DIM1) {(double* items, int length)}

%inline %{
    std::vector<double> my_dvector(double* items, int length)
    {
        return std::vector<double> {items, items + length};
    }
%}

%template (IVector) std::vector<int>;
ADD_NUMPY_ARRAY_INTERFACE(long, IVector, std::vector<int>);

%template (UIVector) std::vector<unsigned int>;
ADD_NUMPY_ARRAY_INTERFACE(long, UIVector, std::vector<unsigned int>);

%{
#define SWIG_FILE_WITH_INIT
#include "Parameters.h"
#include "Simulator.h"
%}

%include "numpy.i"

%init %{
import_array();
%}

%shared_ptr(Parameters)

%apply (double* IN_ARRAY, int DIM1) {(double* FinalTimes_, int final_times_len)}

%extend Parameters {
    Parameters(double Dsize_, double StepSize_,
                double * FinalTimes_, int final_times_len, unsigned long _ic_p)
            {
                std::vector<double> vec {FinalTimes_, FinalTimes_ + final_times_len};
                return new Parameters(Dsize_, StepSize_, vec, _ic_p);
            }
}

%extend Simulator{

    void registerPyListener(PyObject * callback)
    {
        self->registerListener(PyCallback(callback));
    }
}

%include "Parameters.h"
%include "Simulator.h"

