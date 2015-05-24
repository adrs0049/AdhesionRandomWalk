#ifndef CELL_DENSITY_H
#define CELL_DENSITY_H

#include "Boundary.h"
#include "BoundaryFactory.h"
#include "PeriodicBoundary.h"
#include "Loop.h"
#include "Parameters.h"
#include <iostream>
#include <algorithm>
#include <memory>

#include "debug.h"

#define VALID_LATTICE(x) ( (unsigned)x>=0 && p->DomainSizeL>=(unsigned)x )

using namespace boundary;



struct CellDensity {
private:
    static const std::string delta;
    static const std::string uniform;

    Boundary* b;
    std::shared_ptr<Parameters> p;
    std::vector<unsigned int> DensityVector;
    bool init_flag = false;

    void reinit(std::string initial_condition)
    {
            tearDown();
            init(initial_condition);
    }

    void init() { init("Delta"); }

    void init(std::string initial_condition)
    {
        b = BoundaryFactory::createBoundary ( BoundaryFactory::periodic );
        init_flag = true;

        if (initial_condition == uniform)
            init_uniform();
        else if (initial_condition == delta)
            init_delta();
        else {
            std::cerr << "Unknown initial condition: "
                        << initial_condition << std::endl;
            std::cerr << "Aborting!" << std::endl;
            exit(1);
        }
    }

    void tearDown() {
        b = 0;
        p = 0;
        DensityVector.clear();
        init_flag = false;
    }

    void init_uniform()
    {
        std::cout << "init uniform" << std::endl;
        DensityVector.resize( p->getDomainSizeL(), 1);
    }

    void init_delta()
    {
        std::cout << "init delta at location " << p->getDomainSizeL()/2 
          << " with " << p->getNumberOfCells() << " number of cells" 
          << std::endl;

        DensityVector.resize ( p->getDomainSizeL(), 0 );

        DensityVector[DensityVector.size()/2]=p->getNumberOfCells()/2;
        DensityVector[DensityVector.size()/2+1]=p->getNumberOfCells()/2;

        std::cout << "DensityVector[ " << DensityVector.size()/2 
                  << " ] =" << DensityVector[DensityVector.size()/2] 
                  << std::endl;
    }

public:
    CellDensity() : b(0), p(0)
    {}

    CellDensity ( std::shared_ptr<Parameters>  p_)
    : b(nullptr), p ( p_ )
    {
        init();
    }

    CellDensity ( std::shared_ptr<Parameters>  p_,
                  std::string initial_condition)
    : b(nullptr), p ( p_ )
    {
        init(initial_condition);
    }

    void update(std::shared_ptr<Parameters> p_,
                std::string initial_condition)
    {
        init_flag = false;
        p = p_;
        reinit(initial_condition);
    }

    bool checkInit() { return init_flag; }
    bool forceInit() { init(); return init_flag; }

    std::shared_ptr<Parameters> getParameters()
    {
        return p;
    }

    unsigned int front() { return DensityVector.front(); }
    unsigned int back() { return DensityVector.back(); }

    void RightShift(int x)
    {
        ASSERT(x<p->getDomainSizeL(), "");

        //b->applyCondition ( x, p->getDomainSizeL() );
        std::cout << "RightShift: " << x << std::endl;

        ASSERT ((unsigned)x<DensityVector.size()-1, "x "<< x << " invalid \
                density vector index!");

        ASSERT(x>=0, "");

        DensityVector[x]-= (DensityVector[x]>0) ? 1 : 0;
        DensityVector[x+1]+=1;
    }

    void LeftShift(int x)
    {
        ASSERT(x<p->getDomainSizeL(), "Coordinate=" << x << " is not valid!");

        //b->applyCondition ( x, p->getDomainSizeL() );
        std::cout << "LeftShift: " << x << std::endl;

        ASSERT ((unsigned)x<DensityVector.size(), "x "<< x << " invalid \
                density vector index!");

        ASSERT(x>0, "invalid argument for LeftShift");

        DensityVector[x]-= (DensityVector[x]>0) ? 1 : 0;
        DensityVector[x-1]+=1;
    }

    std::size_t size() { return DensityVector.size(); }

    void SetParameters ( std::shared_ptr<Parameters> _p )
    {
        p = _p;
        // FIXME
        //reinit();
    }

    void print() {
      std::cout << "DensityVector=(";
      for (auto& p : DensityVector)
        std::cout << p << ", ";
      std::cout << ")" << std::endl;
    }

    void print ( int c ) {
    }

    unsigned int get(int x) { return getDensity(x); }

    unsigned int getDensity ( int x ) {
        ASSERT(init_flag, "Cells has to be initialized first!");
        //std::cout << "getDensity: "<< x << " Density= " << DensityVector[x]
        // <<std::endl;
        //std::cout << "DL=" << p->DomainSizeL <<std::endl;

        //assert(VALID_LATTICE(b->applyCondition(x, p->DomainSizeL));

        //assert(b->applyCondition(x, p->DomainSizeL) >=0 &&
// b->applyCondition(x, p->DomainSizeL)<=p->DomainSizeL  );
        //std::cout << "x="<<x<<std::endl;
        b->applyCondition ( x, p->getDomainSizeL() );
        //std::cout << "x="<<x<<std::endl;
        //std::cout << "sz="<<DensityVector.size()<<std::endl;
        ASSERT ((unsigned)x<DensityVector.size(), "x "<< x << " invalid \
                density vector index!");
        return DensityVector[x];
    }

    std::vector<unsigned int> getDensityVector()
    {
        return DensityVector;
    }
/*
    std::vector<unsigned int>& getDensityVector()
    {
        return DensityVector;
    }
*/
};

#endif

