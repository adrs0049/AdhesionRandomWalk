#ifndef PROBABILITY_H
#define PROBABILITY_H

#include <iostream>
#include <array>

#include "Cell.h"
#include "Parameters.h"
#include "debug.h"
#include <memory>

#define CHECK_PROB(x) ( x >= 0.0 && x<= 1.0  )
#define RIGHT 1
#define LEFT 0

#define CLAMP(x) ( std::max(0.0, std::min(1.0, x))  )

class Prob
{
public:
    using prob_type = std::array<double, 2>;
    
    Prob() 
    : c(0), p(0), CarryingCapacity(10.0)
    {}
    
    Prob ( std::shared_ptr<Cells> c_ ) : c ( c_ ), p(0), CarryingCapacity(10.0) {
        p = c->getParameters();
        SetA();
        SetB();
        omega_constant = 1.0 / ( 2.0 * p->SensingRadiusL );
    }
   
    void update( std::shared_ptr<Cells> c_)
    {
        c = c_;
        p = c->getParameters();
        omega_constant = 1.0 / ( 2.0 * p->SensingRadiusL );
    }
   
    //~Prop() = default; 

    const prob_type getProb( unsigned int x ) {
	double a_temp = a;
        double b_temp = b * Adhesion ( x );

        //std::cout << "T-="<<a_temp-b_temp<<" T+="<< a_temp+b_temp 
	//	  << " (T+ + T-)=" << 2*a_temp << " a=" << a_temp << " b="  
	//	  << b_temp << std::endl;

        ASSERT ( CHECK_PROB ( a_temp+a_temp ), "2a=" << 2*a_temp );
	
	return { CLAMP(a_temp - b_temp) , CLAMP(a_temp + b_temp) };
    }

    const prob_type operator() ( unsigned int x ) {
    	return getProb(x);
    }

private:
    void SetA() {
        a = p->Diffusion * p->TimeStep / ( p->StepSize * p->StepSize );
    }

    void SetB() {
        b = p->Adhesion * p->TimeStep / ( 2.0 * p->StepSize * p->StepSize );
    }

    double omega ( int r ) {
        return omega_constant;
    }
    double adhesivity ( int r ) {
        return c->getDensity ( r );
    }
    double space ( int r ) {
        return std::max(0.0, 1.0 - (c->getDensity(r) / CarryingCapacity) );
    }

    double Adhesion ( int x ) {
        double total {0.0};
        for ( auto i : range<unsigned> ( 0, p->SensingRadiusL ) )
            total+= ( space ( x+i ) * omega ( +i ) * adhesivity ( x+i ) -
                      space ( x-i ) * omega ( -i ) * adhesivity ( x-i ) );

        //std::cout << "Total=" << total << std::endl;
        return total;
    }

    double CarryingCapacity;
    double omega_constant;
    double a, b;
    std::shared_ptr<Cells> c;
    std::shared_ptr<Parameters> p;
};

#endif
