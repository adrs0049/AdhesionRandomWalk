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
%include <typemaps.i>
%include <std_vector.i>
%include <std_string.i>
%include <std_shared_ptr.i>

%template (DVector) std::vector<double>;
%template (IVector) std::vector<int>;
%template (UIVector) std::vector<unsigned int>;

%shared_ptr(Parameters)

%{
#include "Parameters.h"
#include "Simulator.h"
%}

%include "Parameters.h"
%include "Simulator.h"

