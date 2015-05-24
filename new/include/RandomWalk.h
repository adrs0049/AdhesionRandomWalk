#ifndef RANDOM_WALK_H
#define RANDOM_WALK_H

#include <iostream>
#include <cmath>
#include "ParkMiller.h"
#include "CellDensity.h"
#include "Parameters.h"
//#include "Probability.h"
#include "chronos.h"
#include <memory>
#include "make_unique.h"

#define RIGHT_INC (+1)
#define LEFT_INC (-1)

#define CLAMP_ZERO(x) (if (x<0) std::max(0, x))

class RandomWalk
{
public:
    RandomWalk() 
    : state(nullptr), param(nullptr)
    {}

    RandomWalk( std::shared_ptr<Parameters> _param )
    : state(std::make_shared<CellDensity>(_param)),
    param(_param),
    //p(std::make_unique<Prob>(c)),
    rand ( 0.0, 1.0 ), 
    drand ( 0, param->getNumberOfCells()-1 )
    //propensities(2*(param->getNumberOfCells()-1), 0.0)
    {
          }   
    
    ~RandomWalk() {}

    void update(std::shared_ptr<Parameters> _param)
    {
        //c->update(param);
        //p->update(c);
    }
    
    std::vector<unsigned int> getPath()
    {
        return state->getDensityVector();
    }
 
    void GeneratePath() 
    {
        try {

            // state variables of simulation
            time = 0.0;
            steps=0;

            print_info();

            std::cout << "Setup simulation..." << std::endl;

            // ensure all things are set and updated
            update();

            std::cout << "Starting timer..." << std::endl;
            chronos::Chronos timer;

            std::cout << "Starting simulation..." << std::endl;

            do {
                // before each step update the propensities
                updatePropensity();

                steps++;

                Step();

                std::cout << "Time:" << time << " step: " << steps << std::endl;
            
                //state->print();

            } while (time < param->getFinalTime() );

            std::cout << "Simulation complete. The total number of steps is " << steps << std::endl;

            print_info();

        } catch (const std::exception& exception) {
            std::cerr << "ERROR: " << exception.what() << std::endl;
        }
    }

    void Step()
    {
        auto r1 = rand();
        auto r2 = rand();

        std::cout << "r1: " << r1 << " r2: " << r2 << std::endl;

        // compute propensity
        double a0 = getPropensitySum();
        ASSERT(a0!=0.0, "total propensity is zero. Error!");

        std::cout << "a0: " << a0 << std::endl;

        time += 1.0 / a0 * std::log(1.0 / r1);
        
        long k {0};
        double ss {0.0};
        auto r2a0 = r2 * a0;
    
        std::cout << "step: " << steps << " r2a0:" << r2a0
                  << std::endl;

        while (ss <= r2a0 && (k < param->getDomainSizeL()-1))
        {
            k++;
            ss+=getPropensity(k, 0); // d * c->getDensity(k);
        }

        //std::cout << "sum: "<<ss << std::endl;
        if (ss > r2a0)
        {
            // Move to the right
            state->RightShift(k);
            
        } else {
            // Hit the maximum number in counting 
            // Now find a LeftShift reaction
            k=1;
            while (ss < r2a0 && k < param->getDomainSizeL())
            {
                k++;
                ss+=getPropensity(k, 1); // d * c->getDensity(k);
            }
            //std::cout<<"k: "<<k<<" ss: "<< ss<<std::endl;
            
            // The simulation has to find a Rxn
            ASSERT(k<param->getDomainSizeL(), "Simulation failed did not find a Rxn to update!");

            state->LeftShift(k);
        }

        // Tolerance to account for slight rounding errors
        ASSERT(ss<=a0+1E-3, "Propensity sum (" << ss << ") can't be larger than the propensity sum a0 (" << a0 << ") !");

    }

    void update() 
    {
      propensity_stride = param->getDomainSizeL() - 2;
      NumberOfReactions = 2 * (param->getDomainSizeL() - 1);
      //computeAllPropensities();
      state->print();
    }

    std::shared_ptr<CellDensity> getCells()
    {
        return state;
    }
    
private:
  
    void print_info()
    {
        std::cout << "Starting Gillespie's SSA for a simulation a space-jump "
                      << "process. The space-jump process includes diffusion and "
                      << "drift." << std::endl;

        std::cout << "This simulation is run with " << param->getNumberOfCells()
                      << " cells. The final time of the simulation is "
                      << param->getFinalTime() << "." << std::endl;

        std::cout << "Courant Number=" << 1.0 / param->getDiffusionSim()
                  << " Lambda=" << param->getLambda()
                  << " Diffusion=" << param->getDiffusion()
                  << " Drift=" << param->getDrift() << std::endl
                  << " Drift Sim=" << param->getDriftSim()
                  << " DiffusionRate=" << param->getDiffusionSim()
                  << " DriftRate=" << param->getDriftSim()
                  << std::endl;
    }

    void computeAllPropensities()
    {
        // implement this better don't recompute the adhesion each time again
        propensities.resize(NumberOfReactions, 0);

        for (long k = 0; k < propensity_stride; k++)
        {
            propensities.at(k) = getTransitionRate(k, 0);
            propensities.at(propensity_stride + k) = getTransitionRate(k, 1);
            //std::cout << "Filling " << k << " and " << propensity_stride + k << std::endl;
        }

        /*
        for (auto& propensity : propensities)
          propensity = param->getDiffusionSim();
        */
    }

    double getPropensity(int coordinate, int flag)
    {
        ASSERT(coordinate >= 0 && coordinate < param->getDomainSizeL(), \
            "Lattice coordinate= " << coordinate << " is invalid. Valid \
            lattice coordinate range is (" << 0 << ", " << \
              param->getDomainSizeL() << ").");

        ASSERT((flag * propensity_stride + coordinate) >=0 \
            && (flag * propensity_stride + coordinate)<propensities.size(), \
            "RxnIndex=" << flag * propensity_stride + coordinate 
            << " is invalid. The valid range is (" << 0 << ", " 
            << propensities.size() << "). Coordinate=" << coordinate
            << " flag=" << flag << ".");
      /*
        std::cout << "propensity ( " << coordinate << " , " << flag << " ):" 
          << propensities.at(flag * propensity_stride + coordinate) <<
          " state ( " << coordinate << ")=" << 
            state->get(coordinate) << std::endl;
       */

        // TODO save the complete propensity in a vector 
        // so we dont have to recompute it each
        ASSERT(propensities.at(flag * propensity_stride + coordinate)>=0.0,
               "Propensity at index " << flag * propensity_stride + coordinate <<
               " is negative. Propensities have to be non-negative");

        return propensities.at(flag * propensity_stride + coordinate) * 
                state->get(coordinate);
    }

    double getPropensitySum()
    {
      double ret = getPropensity(0, 0);
      for (long k = 0; k < param->getDomainSizeL() - 1; k++)
      {
        ret += getPropensity(k, 0);
        ret += getPropensity(k, 1);
      }
      ret += getPropensity(param->getDomainSizeL()-1, 1);

      return ret;
    }

    double ComputePropensity()
    {
        return param->getDiffusionSim() * ( 2.0 * param->getNumberOfCells() +
            state->front() + state->back());
    }

    void updatePropensity()
    {
        // very simple, improve this!
        computeAllPropensities();
    }

    // get transition rate for left: flag -> 1
    // or right jump: flag -> 0
    double getTransitionRate( int coordinate, int flag )
    {
        switch(flag)
        {
            case 0:
                return param->getLambda() * ( param->getDiffusionSim()
                    + param->getDriftSim() * param->getDiscreteX() * PolarizationVector( coordinate ) );
            case 1:
                return param->getLambda() * ( param->getDiffusionSim()
                    - param->getDriftSim() * param->getDiscreteX() * PolarizationVector( coordinate ) );
            default:
                ASSERT(false, "ERROR");
                break;
        }
    }

    double omega ( int coordinate )
    {
        return 1.0;
    }

    double adhesivity ( int coordinate )
    {
        return state->getDensity ( coordinate );
    }

    double space ( int coordinate )
    {
        return 1.0; //std::max(0.0, 1.0 - (state->getDensity(r) / CarryingCapacity) );
    }

    double PolarizationVector ( int coordinate )
    {
        double total {0.0};
        for ( auto offset : range<unsigned> ( 0, param->getSensingRadiusL() ) )
            total+= ( space ( coordinate + offset ) * omega ( +offset ) * adhesivity ( coordinate + offset ) -
                      space ( coordinate - offset ) * omega ( -offset ) * adhesivity ( coordinate - offset ) );

        return total;
    }

    // Variables
    std::shared_ptr<CellDensity> state;
    std::shared_ptr<Parameters> param;
    //std::unique_ptr<Prob> p;
    //Prob p;

    // Propensity vector
    std::vector<double> propensities;
    unsigned long propensity_stride;

    unsigned long steps;
    double time;

    unsigned long right;
    unsigned long left;
    unsigned long NumberOfReactions;
    unsigned long NumberOfAttemptFlips;
    unsigned long NumberOfUnsuccessfulFlips;
    unsigned int Multiplier = 1;
    UniformRandomNumberGenerator rand;
    UniformDiscreteRandomNumberGenerator drand;
};

#endif
