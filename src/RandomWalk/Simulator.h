// simulator class
//
//

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <Loop.h>

#include <ParkMiller.h>

#include <PeriodicBoundary.h>
#include <Boundary.h>
#include <BoundaryFactory.h>

#include "RandomWalk.h"
#include "Parameters.h"
#include <memory>
#include "make_unique.h"

#include "Terminate.h"
//#include "Event.h"
//#include "EventListener.h"

using namespace boundary;

class Simulator
{

public:

    Simulator()
    : param(), TheRandomWalk()
    {}

    Simulator ( std::shared_ptr<Parameters> p )
    : param(p), TheRandomWalk ()
    {
        init(p);
        //std::cout << "DL(sim)=" << p.DomainSizeL << std::endl;
    }

    ~Simulator() = default;

    void init(std::shared_ptr<Parameters> param)
    {
		// DEBUG stuff
        Error::TerminalCatcher::init();

		//std::cout << "Simulator init" << std::endl;
		//std::cout << "Parameters are: ";
		//param->print_info();
        TheRandomWalk = std::make_unique<RandomWalk>(param);
    }

    void update(std::shared_ptr<Parameters> param_)
    {
        param = param_;
        TheRandomWalk->update(param);
    }

    void print() {
        auto path = TheRandomWalk->getCells();
        path->print();
    }

    void print ( int c ) {
        //cells->print ( c );
    }

    void run()
    {
        try {
            TheRandomWalk->GeneratePath();
        }
        catch(const std::exception& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }

    std::vector<unsigned int> getPath()
    {
        return TheRandomWalk->getPath();
    }
    /*
    std::vector<double> getCOM()
    {
        return TheRandomWalk->getCells()->getCellCOM();
    }
    */
    void run ( std::shared_ptr<Parameters> param_ ) {
        setParameters ( param_ );
        run();
    }

    void setParameters( std::shared_ptr<Parameters> param_) {
        update(param_);
    }

private:
    //std::unique_ptr<Cells> cells;
    std::shared_ptr<Parameters> param;
    std::unique_ptr<RandomWalk> TheRandomWalk;
};


