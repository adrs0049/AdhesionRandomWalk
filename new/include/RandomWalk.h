#ifndef RANDOM_WALK_H
#define RANDOM_WALK_H

#include <iostream>
#include <iomanip>
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
    rand ( 0.0, 1.0 ), 
    drand ( 0, param->getNumberOfCells()-1 )
    {
          }   
    
    ~RandomWalk() {}

    void update(std::shared_ptr<Parameters> _param)
    {
        update();
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

            // initialise everything
            computeAllPropensities();

            do {
                // before each step update the propensities
                //updatePropensity();

                steps++;

                Step();

                std::cout << "\rTime:" << std::setw(15) << time 
                          << " Step: " << std::setw(10) << steps;
            
                //state->print();

            } while (time < param->getFinalTime() );

            std::cout << std::endl;
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

        //std::cout << "r1: " << r1 << " r2: " << r2 << std::endl;

        // compute propensity
        double a0 = getPropensitySum();
        ASSERT(a0!=0.0, "total propensity is zero. Error!");

        //std::cout << "a0: " << a0 << std::endl;

        time += 1.0 / a0 * std::log(1.0 / r1);
        
        // start at -1
        long k {-1};
        double ss {0.0};
        auto r2a0 = r2 * a0;
    
        //std::cout << "step: " << steps << " r2a0:" << r2a0
        //          << std::endl;

        // propensity stride is all the reactions to the right
        while (ss <= r2a0 && k < propensity_stride - 1)
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
            k=-1; // we also have propensity_stride left rxns now
            while (ss < r2a0 && k < propensity_stride - 1)
            {
                k++;
                ss+=getPropensity(k, 1); // d * c->getDensity(k);
            }
            
            // The simulation has to find a Rxn
            ASSERT(k<param->getDomainSizeL(), "Simulation failed did not find a Rxn to update!");

            state->LeftShift(k);
        }

        //std::cout<<" k: "<<k<<" ss: "<< ss<< " a0="<< a0 <<std::endl;
        // shift is done now update the propensity vector
        updatePropensity(k);
        //computeAllPropensities();

        // Tolerance to account for slight rounding errors
        ASSERT(ss<=a0+1E-3, "Propensity sum (" << ss << ") can't be larger than the propensity sum a0 (" << a0 << ") !");

    }

    void update() 
    { 
      // propensity_stride = -2 for no flux bc
      propensity_stride = param->getDomainSizeL();
      // FIXME to the selection for different boundary conditions better
      // for no flux boundary conditions
      // NumberOfReactions = 2 * (param->getDomainSizeL() - 1);
      // for periodic boundary conditions
      NumberOfReactions = 2 * param->getDomainSizeL();
      sensing_offset = param->getSensingRadiusL();

      std::cout << "NumberOfReactions= " << NumberOfReactions 
                << " propensity_stride=" << propensity_stride << std::endl;

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
                  << " StepSize=" << param->getDiscreteX()
                  << " Lambda=" << param->getLambda()
                  << " Diffusion=" << param->getDiffusion()
                  << " Drift=" << param->getDrift() << std::endl
                  << " Drift_Sim=" << param->getDriftSim()
                  << " DiffusionRate=" << param->getDiffusionSim()
                  << " DriftRate=" << param->getDriftSim() << std::endl
                  << " SensingRadius=" << param->getSensingRadius()
                  << " SensingRadiusL=" << param->getSensingRadiusL()
                  << std::endl;
    }

    void computeAllPropensities()
    {
        // implement this better don't recompute the adhesion each time again
        propensities.resize(NumberOfReactions, 0);

        for (long k = 0; k < propensity_stride; k++)
        {
            computePropensity(k);

            //propensities.at(k) = getTransitionRate(k, 0);
            //propensities.at(propensity_stride + k) = getTransitionRate(k, 1);
            //std::cout << "Filling " << k << " and " << propensity_stride + k << std::endl;
        }

        /*
        for (auto& propensity : propensities)
          propensity = param->getDiffusionSim();
        */
    }

    // function to wrap around propensity index
    // when using periodic boundary conditions
    bool propensityApplyCondition(long& idx, const long& max_value)
    {
        if (idx < 0)
        {
            auto val = std::abs((float)(idx % max_value));
            idx = max_value - val;
            return true;
        }
        else if (idx >= max_value)
        {
            idx = idx % max_value;
            return true;
        }
        return false;
    }

    // TODO write a test to test if this really works
    void updatePropensity(long coordinate)
    {
        // add 1 to the offset
        long offset = sensing_offset + 1;
        // very simple, improve this!

        //std::cout << "updatePropensity, offset="<<offset << " coordinate="<<coordinate << " k="<<coordinate-offset<<std::endl;

        for (long k = coordinate - offset; k < coordinate + offset; k++)
        {
            //std::cout << "prop update k="<< k<<std::endl;
            // cant pass k to it otherwise it will be changed!!
            // TODO invoke bcs here somehow
            long i {k};
            propensityApplyCondition(i, param->getDomainSizeL());
            computePropensity(i);
        }
    }

    double getPropensity(int coordinate, int flag)
    {
        ASSERT(coordinate >= 0 && coordinate < param->getDomainSizeL(), \
            "Lattice coordinate= " << coordinate << " is invalid. Valid \
            lattice coordinate range is [" << 0 << ", " << \
              param->getDomainSizeL() << ").");

        ASSERT((flag * propensity_stride + coordinate) >=0 \
            && (flag * propensity_stride + coordinate)<propensities.size(), \
            "RxnIndex=" << flag * propensity_stride + coordinate 
            << " is invalid. The valid range is [" << 0 << ", "
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

        // TODO check that this offset is correct
        return propensities.at(flag * ( propensity_stride - 1 ) + coordinate);
    }

    double getPropensitySum()
    {
        double ret {0.0};
        for (const auto& propensity : propensities)
            ret+=propensity;

        return ret;
    }
    
    void computePropensity( long coordinate )
    {
        //std::cout << "compute propensity at " << coordinate << " ";

        //propensities.at(k) = getTransitionRate(k, 0);
        //propensities.at(propensity_stride + k) = getTransitionRate(k, 1);

        // compute polarization at the location only once
        auto polarization = PolarizationVector(coordinate);

        // compute components for transition rates
        double symmetric = param->getDiffusionSim();
        double asymmetric = param->getDriftSim() * param->getDiscreteX() * polarization;

        //std::cout << "right=" << (symmetric + asymmetric);
        //std::cout << "left=" << (symmetric - asymmetric) << std::endl;

        ASSERT((symmetric-asymmetric)>=0.0, "");

        propensities.at(coordinate) = state->get(coordinate) * param->getLambda() * (symmetric + asymmetric);
        // check that this offset is correct here??
        propensities.at(propensity_stride + coordinate - 1) = state->get(coordinate) * param->getLambda() * (symmetric - asymmetric);
    }

    double omega ( long coordinate )
    {
        return 1.0;
    }

    double adhesivity ( long coordinate )
    {
        return state->getDensity ( coordinate );
    }

    double space ( long coordinate )
    {
        return 1.0; //std::max(0.0, 1.0 - (state->getDensity(r) / CarryingCapacity) );
    }

    double PolarizationVector ( long coordinate )
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
    long propensity_stride;
    unsigned long sensing_offset;

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
