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
    : c(), param()
    {}

    RandomWalk( std::shared_ptr<Parameters> _param )
    : c(std::make_shared<CellDensity>(_param)),
    param(_param),
    //p(std::make_unique<Prob>(c)),
    rand ( 0.0, 1.0 ), 
    drand ( 0, param->getNumberOfCells()-1 )
    //propensity(param->getNumberOfCells(), 0.0)
    {}   
    
    ~RandomWalk() {}

    void update(std::shared_ptr<Parameters> _param)
    {
        //c->update(param);
        //p->update(c);
    }
    
    std::vector<unsigned int> getPath()
    {
        return c->getDensityVector();
    }
 
    void GeneratePath() {

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
            
            c->print();

        } while (time < param->getFinalTime() );
    }

    void Step() {
        auto r1 = rand();
        auto r2 = rand();

        steps++;

        std::cout << "r1: " << r1 << " r2: " << r2 << std::endl;

        // compute propensity
        double a0 = ComputePropensity();

        std::cout << "a0: " << a0 << std::endl;

        time += 1.0 / a0 * std::log(1.0 / r1);
        
        long k {0};
        double ss {0.0};
        auto r2a0 = r2 * a0;
        auto d = DiffusionRateConstant();
    
        std::cout << "step: " << steps << " r2a0:" << r2a0
                  << " d: " << d << std::endl;

        while (ss <= r2a0 && (k < param->getDomainSizeL()-1))
        {
            k++;
            ss+=d * c->getDensity(k);
        }

        std::cout << "sum: "<<ss << std::endl;
        if (ss > r2a0)
        {
            // Move to the right
            c->RightShift(k);
            
        } else {
            // Hit the maximum number in counting 
            k=1;
            while (ss < r2a0 && k < param->getDomainSizeL())
            {
                k++;
                ss+=d * c->getDensity(k);
            }
            std::cout<<"k: "<<k<<" ss: "<< ss<<std::endl;
            c->LeftShift(k);
        }

    }

    std::shared_ptr<CellDensity> getCells()
    {
        return c;
    }
    
private:
   
    double DiffusionRateConstant()
    {
        return param->getDiffusion() / 
            (param->getDiscreteX() * param->getDiscreteX());
    }

    double ComputePropensity()
    {
        return DiffusionRateConstant() * ( 2.0 * param->getNumberOfCells() +
            c->front() + c->back());
    }

    void updatePropensity()
    {

    }

    std::shared_ptr<CellDensity> c;
    std::shared_ptr<Parameters> param;
    //std::unique_ptr<Prob> p;
    //Prob p;

    // Propensity vector
    //std::vector<double> propensity;

    unsigned long steps;
    double time;

    unsigned long right;
    unsigned long left;
    unsigned long NumberOfAttemptFlips;
    unsigned long NumberOfUnsuccessfulFlips;
    unsigned int Multiplier = 1;
    UniformRandomNumberGenerator rand;
    UniformDiscreteRandomNumberGenerator drand;
};

#endif
