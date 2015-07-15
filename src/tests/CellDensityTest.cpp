#include <iostream>
#include "Parameters.h"
#include "StateVector.h"
#include <memory>
#include "debug.h"

#include "Terminate.h"

int main()
{
    Error::TerminalCatcher::init();

    auto p = std::make_shared<Parameters>(5, 0.1, 0.1, 1.0);
    auto c = std::make_shared<stateVector<unsigned int>>(p);

    c->print();

    c->LeftShift(10);

    c->print();

    for (std::size_t i = 0; i < 9; i++)
        ASSERT(c->getDensity(i)==1, "");
    for (std::size_t i = 11; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    ASSERT(c->getDensity(9)==2, "");
    ASSERT(c->getDensity(10)==0, "");

    c->print();

    c->RightShift(9);

    c->print();

    for (std::size_t i = 0; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    c->print();

    c->RightShift(22);

    for (std::size_t i = 0; i < 22; i++)
        ASSERT(c->getDensity(i)==1, "");
    for (std::size_t i = 24; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    ASSERT(c->getDensity(22)==0, "");
    ASSERT(c->getDensity(23)==2, "");

    c->print();

    c->LeftShift(23);
    c->print();

    for (std::size_t i = 0; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    std::cout << "Testing shifts close and on the boundary" << std::endl;
    // try to see how shifts on the boundary go
    std::cout << "Right shift at 0" << std::endl;
    c->RightShift(0);
    c->print();

    ASSERT(c->getDensity(0)==0, "");
    ASSERT(c->getDensity(1)==2, "");
    for (std::size_t i = 2; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    std::cout << "Restore" << std::endl;
    c->LeftShift(1);
    c->print();

    for (std::size_t i = 0; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    std::cout << "Left shift at 0" << std::endl;
    c->LeftShift(0);
    c->print();

    ASSERT(c->getDensity(0)==0, "");
    ASSERT(c->getDensity(c->size()-1)==2, "");
    for (std::size_t i = 1; i < c->size()-1; i++)
        ASSERT(c->getDensity(i)==1, "");

    std::cout << "Restore" << std::endl;
    c->RightShift(p->getDomainSizeL()-1);
    c->print();

    for (std::size_t i = 0; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    std::cout << "Shift Left on last" << std::endl;
    c->LeftShift(c->size()-1);
    c->print();

    for (std::size_t i = 0; i < c->size()-2; i++)
        ASSERT(c->getDensity(i)==1, "");

    ASSERT(c->getDensity(c->size()-2)==2, "");
    ASSERT(c->getDensity(c->size()-1)==0, "");

    c->RightShift(c->size()-2);
    c->print();

    for (std::size_t i = 0; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    std::cout << "Right shift on last" << std::endl;
    c->RightShift(c->size()-1);
    c->print();

    ASSERT(c->getDensity(0)==2, "");
    ASSERT(c->getDensity(c->size()-1)==0, "");

    for (std::size_t i = 1; i < c->size()-1; i++)
        ASSERT(c->getDensity(i)==1, "");

    c->LeftShift(0);
    c->print();

    for (std::size_t i = 0; i < c->size(); i++)
        ASSERT(c->getDensity(i)==1, "");

    return 0;
}

