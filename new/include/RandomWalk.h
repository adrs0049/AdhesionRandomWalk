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
        // ensure all things are set and updated
        update();

        // call the timer
        time = 0.0;
        steps=0;
        chronos::Chronos timer;
        std::cout << "Generating Path up to time "
                  << param->getFinalTime() << "." << std::endl;
        std::cout << "Courant Number=" << param->getDiscreteX() *
            param->getDiscreteX() / param->getDiffusion() << std::endl;

        do {
            Step();
           
            std::cout << "Time:" << time << " step: " << steps << std::endl;
            
            state->print();

        } while (time < param->getFinalTime() );
    }

    void Step() {
        auto r1 = rand();
        auto r2 = rand();

        steps++;

        std::cout << "r1: " << r1 << " r2: " << r2 << std::endl;

        // compute propensity
        double a0 = getPropensitySum();

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

    }

    void update() 
    {
      propensity_stride = param->getDomainSizeL() - 2;
      NumberOfReactions = 2 * (param->getDomainSizeL() - 1);
      computeAllPropensities();
      state->print();
    }

    std::shared_ptr<CellDensity> getCells()
    {
        return state;
    }
    
private:
  
    void computeAllPropensities()
    {
        propensities.resize(NumberOfReactions, 0);
        for (auto& propensity : propensities)
          propensity = DiffusionRateConstant();
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
        // so we dont have to recompute it each time
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

    double DiffusionRateConstant()
    {
        return param->getDiffusion() / 
            (param->getDiscreteX() * param->getDiscreteX());
    }

    double ComputePropensity()
    {
        return DiffusionRateConstant() * ( 2.0 * param->getNumberOfCells() +
            state->front() + state->back());
    }

    void updatePropensity()
    {

    }

    double getTransitionRate()
    {}

    

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
