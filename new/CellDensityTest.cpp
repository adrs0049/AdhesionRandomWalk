#include <iostream>
#include "Parameters.h"
#include "CellDensity.h"
#include <memory>
#include "debug.h"

const std::string BoundaryFactory::no_flux("NoFlux");
const std::string BoundaryFactory::periodic("Periodic");

const std::string CellDensity::delta("Delta");
const std::string CellDensity::uniform("Uniform");

int main() 
{
  auto p = std::make_shared<Parameters>(10, 0.1, 0.1, 10.0);
  auto c = std::make_shared<CellDensity>(p, "Uniform");

  c->print();

  c->LeftShift(10);

  c->print();

  for (int i = 0; i < 9; i++)
    ASSERT(c->getDensity(i)==1, "");
  for (int i = 11; i < c->size(); i++)
    ASSERT(c->getDensity(i)==1, "");

  ASSERT(c->getDensity(9)==2, "");
  ASSERT(c->getDensity(10)==0, "");

  c->print();

  c->RightShift(9);

  c->print();

  for (int i = 0; i < c->size(); i++)
    ASSERT(c->getDensity(i)==1, "");

  c->print();

  c->RightShift(22);

  for (int i = 0; i < 22; i++)
    ASSERT(c->getDensity(i)==1, "");
  for (int i = 24; i < c->size(); i++)
    ASSERT(c->getDensity(i)==1, "");

  ASSERT(c->getDensity(22)==0, "");
  ASSERT(c->getDensity(23)==2, "");

  c->print();

  return 0;
}

