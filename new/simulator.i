// simulator.i - SWIG interface

%include boundary.i
%include <typemaps.i>
%include <std_vector.i>
%include <std_string.i>
%include <std_shared_ptr.i>

%shared_ptr(Parameters)

%module simulator 
%{
#include "Parameters.h"
#include "CellDensity.h"
#include "RandomWalk.h"
#include "Simulator.h"
%}

%template (DVector) std::vector<double>;
%template (IVector) std::vector<int>;
%template (UIVector) std::vector<unsigned int>;

%include "Parameters.h"
%include "CellDensity.h"
%include "RandomWalk.h"
%include "Simulator.h"

