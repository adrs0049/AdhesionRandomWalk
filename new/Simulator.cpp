
#include "Simulator.h"
#include "Loop.h"
#include <algorithm>
#include <iostream>

template<typename T>
const std::string stateVector_impl<T>::delta("Delta");
template<typename T>
const std::string stateVector_impl<T>::uniform("Uniform");

const std::string BoundaryFactory::no_flux("NoFlux");
const std::string BoundaryFactory::periodic("Periodic");

