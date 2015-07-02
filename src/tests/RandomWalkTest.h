// test
#include <cxxtest/TestSuite.h>

#include <iostream>
#include "Parameters.h"
#include "RandomWalk.h"
#include <memory>
#include "debug.h"

#include <iterator>
#include <random>

#include "Terminate.h"

const std::string BoundaryFactory::no_flux("NoFlux");
const std::string BoundaryFactory::periodic("Periodic");

template<typename T>
const std::string stateVector_impl<T>::delta("Delta");

template<typename T>
const std::string stateVector_impl<T>::uniform("Uniform");

void printVec(std::vector<double> pVec)
{
    std::cout << "Propensity size = " << pVec.size() << std::endl;
    std::cout << "pVec=(";
    std::copy(pVec.begin(), pVec.end(), std::ostream_iterator<double>(std::cout, ", "));
    std::cout << ")." << std::endl;
}

class RandomWalkTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
    std::shared_ptr<RandomWalk> rw;

    double tol = 1E-5;

public:

    void setUp(void)
    {
        std::cerr << "setup" << std::endl;
        p = std::make_shared<Parameters>(5, 0.1, 0.1, 10);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);

        p->Check();

        rw = std::make_shared<RandomWalk>(p);
        rw->setup();
    }

    void tearDown(void)
    {
        std::cerr << "TearDown" << std::endl;

        p.reset();
        rw.reset();
    }

    void testBasic(void)
    {
        unsigned long Dsz = 50;
        auto propensity_stride = rw->getStride();
        TS_ASSERT_EQUALS(propensity_stride, Dsz);

        auto& pVec = rw->getProp();
        TS_ASSERT_EQUALS(pVec.size(), 2*Dsz);
        for (auto& vec : pVec)
            TS_ASSERT_DELTA(vec, 0.0, tol);
    }

    void testComputeBasic(void)
    {
        auto& pVec = rw->getProp();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, rw->getStride()-1);

        int number_of_runs = 1000;

        for (int run = 0; run < number_of_runs; run++)
        {

        int x = dis(gen);
        std::cout << "Testing to set propensities at " << x << std::endl;

        rw->computePropensity(x);
        printVec(rw->getProp());

        auto propensity_stride = rw->getStride();
        for(int i = 0; i < propensity_stride; i++)
        {
            if (i == x) {
                TS_ASSERT_DELTA(pVec[i], 100.0, tol);
                TS_ASSERT_DELTA(rw->getPropensity(i, 0), 100.0, tol);
                continue;
            }
            TS_ASSERT_DELTA(pVec[i], 0.0, tol);
            TS_ASSERT_DELTA(rw->getPropensity(i, 0), 0.0, tol);
        }

        for(int i = propensity_stride; i < pVec.size(); i++)
        {
            if (i == x + propensity_stride) {
                TS_ASSERT_DELTA(pVec[i], 100.0, tol);
                TS_ASSERT_DELTA(rw->getPropensity(i - propensity_stride, 1), 100.0, tol);
                continue;
            }
            TS_ASSERT_DELTA(pVec[i], 0.0, tol);
            TS_ASSERT_DELTA(rw->getPropensity(i - propensity_stride, 1), 0.0, tol);
        }

        auto prop_at_x = rw->getPropensity(x);
        std::cout << "right=" << prop_at_x[0] << " left=" << prop_at_x[1]
                  << std::endl;

        TS_ASSERT_DELTA(prop_at_x[0], 100.0, tol);
        TS_ASSERT_DELTA(prop_at_x[1], 100.0, tol);

        TS_ASSERT_DELTA(prop_at_x[0], rw->getPropensity(x, 0), tol);
        TS_ASSERT_DELTA(prop_at_x[1], rw->getPropensity(x, 1), tol);
        // reset prop vector
        rw->setup();

        }
    }

    void testPropAtZero(void)
    {
        auto& pVec = rw->getProp();

        int x = 0;
        std::cout << "Testing to set propensities at " << x << std::endl;

        rw->computePropensity(x);
        printVec(rw->getProp());

        auto propensity_stride = rw->getStride();
        for(int i = 0; i < propensity_stride; i++)
        {
            if (i == x) {
                TS_ASSERT_DELTA(pVec[i], 100.0, tol);
                TS_ASSERT_DELTA(rw->getPropensity(i, 0), 100.0, tol);
                continue;
            }
            TS_ASSERT_DELTA(pVec[i], 0.0, tol);
            TS_ASSERT_DELTA(rw->getPropensity(i, 0), 0.0, tol);
        }

        for(int i = propensity_stride; i < pVec.size(); i++)
        {
            if (i == x + propensity_stride) {
                TS_ASSERT_DELTA(pVec[i], 100.0, tol);
                TS_ASSERT_DELTA(rw->getPropensity(i - propensity_stride, 1), 100.0, tol);
                continue;
            }
            TS_ASSERT_DELTA(pVec[i], 0.0, tol);
            TS_ASSERT_DELTA(rw->getPropensity(i - propensity_stride, 1), 0.0, tol);
        }

        auto prop_at_x = rw->getPropensity(x);
        std::cout << "right=" << prop_at_x[0] << " left=" << prop_at_x[1]
                  << std::endl;

        TS_ASSERT_DELTA(prop_at_x[0], 100.0, tol);
        TS_ASSERT_DELTA(prop_at_x[1], 100.0, tol);

        TS_ASSERT_DELTA(prop_at_x[0], rw->getPropensity(x, 0), tol);
        TS_ASSERT_DELTA(prop_at_x[1], rw->getPropensity(x, 1), tol);
    }

    void testSetAll(void)
    {
        auto& pVec = rw->getProp();

        rw->computeAllPropensities();

        for (auto& vec : pVec)
            TS_ASSERT_DELTA(vec, 100.0, tol);
    }

};

