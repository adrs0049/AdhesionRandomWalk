#ifndef RANDOM_WALK_H
#define RANDOM_WALK_H

#include <iostream>
#include "ParkMiller.h"
#include "Cell.h"
#include "Parameters.h"
#include "Probability.h"
#include "chronos.h"
#include <memory>
#include "make_unique.h"

#define RIGHT_INC (+1)
#define LEFT_INC (-1)

class RandomWalk
{
public:
    RandomWalk() 
    : c(), p()
    {}

    RandomWalk( std::shared_ptr<Parameters> param ) 
    : c(std::make_shared<Cells>(param)), 
    p(std::make_unique<Prob>(c)), 
    rand ( 0.0, 1.0 ), 
    drand ( 0, param->NumberOfCells-1 ) 
    {}   
    
    ~RandomWalk() {}

    void update(std::shared_ptr<Parameters> param)
    {
        c->update(param);
        p->update(c);
    }
    
    std::vector<unsigned int> getPath()
    {
        return c->getDensityVector();        
    }
 
    void GeneratePath() {

        // call the timer
        chronos::Chronos timer;
        std::cout << "Generating Path up to time " << c->getParameters()->FinalTime << "." << std::endl;
        std::cout << "TimeStep=" << c->getParameters()->TimeStep << std::endl;
        std::cout << "Courant Number=" << c->getParameters()->StepSize * c->getParameters()->StepSize / c->getParameters()->Diffusion << std::endl;
        
        unsigned long NumberOfSteps = c->getParameters()->FinalTime / c->getParameters()->TimeStep;
        unsigned long CurrentStep = 0;
      
        do {
            Step();
           
            std::cout << "Step:" << CurrentStep;
            std::cout << " Flips: " << (NumberOfAttemptFlips - NumberOfUnsuccessfulFlips) << "/" \
                  << NumberOfAttemptFlips << " Right: " << right 
                  << " Left:" << left << " Bias: " 
                  << static_cast<long>(right) - static_cast<long>(left) << std::endl;
                  
        } while ( ++CurrentStep < NumberOfSteps ); 
    }

    void Step() {
        unsigned long CurrentStep = 0;
        NumberOfAttemptFlips = Multiplier * c->getParameters()->NumberOfCells;
        NumberOfUnsuccessfulFlips=0;
        right = left = 0;
        
        do {
            SingleStep();
            
        } while ( ++CurrentStep < NumberOfAttemptFlips);
        
    }

    void SingleStep() {
        // choose random cell
        auto id = drand();
        auto& cell = c->getCell ( id );

        //std::cout << "id = " << id << " cell.id="<<cell.id<<std::endl;
        assert ( cell.id == id );

        //std::cout << "CELL.id=" << id << " CELL.com=" << cell.com << std::endl;

        auto com = cell.com;
        //std::cout << "com=" << com << std::endl;

        // get probabilities
        auto prob = p->getProb( com );
        
	ASSERT ( CHECK_PROB ( prob[LEFT]  ), "a+b=" << prob[LEFT] );
	ASSERT ( CHECK_PROB ( prob[RIGHT] ), "a-b=" << prob[RIGHT] );
	
	auto r = rand();

        // update location
        if ( r < prob[LEFT] ) {
            //std::cout << "LEFT" << std::endl;
            left++;
            c->updatePosition ( cell, LEFT_INC );
        } else if ( r < prob[LEFT] + prob[RIGHT] ) {
            //std::cout << "RIGHT" << std::endl;
            right++;
            c->updatePosition ( cell, RIGHT_INC );
        } else
        {
            NumberOfUnsuccessfulFlips++;            
        }

        //std::cout << "CELL.com=" << cell.com << std::endl;
        //std::cout << "DONE" << std::endl;
        // else nothing happens
    }

    std::shared_ptr<Cells> getCells()
    {
        return c;
    }
    
private:
    
    std::shared_ptr<Cells> c;
    std::unique_ptr<Prob> p;
    //Prob p;

    unsigned long right;
    unsigned long left;
    unsigned long NumberOfAttemptFlips;
    unsigned long NumberOfUnsuccessfulFlips;
    unsigned int Multiplier = 1;
    UniformRandomNumberGenerator rand;
    UniformDiscreteRandomNumberGenerator drand;
};

#endif
