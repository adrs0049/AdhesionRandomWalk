// simulator.i - SWIG interface

%include "exception.i"

%exception {
    try {
        $action
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

%include boundary.i
%include <typemaps.i>
%include <std_vector.i>
%include <std_string.i>
%include <std_shared_ptr.i>

%shared_ptr(Parameters)

%module simulator 
%{
#include "Parameters.h"
#include "Simulator.h"
%}

%template (DVector) std::vector<double>;
%template (IVector) std::vector<int>;
%template (UIVector) std::vector<unsigned int>;

%include "Parameters.h"
%include "Simulator.h"

