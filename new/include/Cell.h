#ifndef CELL_H
#define CELL_H

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

struct Cell {
    Cell () {}
    Cell ( unsigned long com )
        : com ( com ), id ( 0 ) {}
    Cell ( unsigned long com, unsigned long i ) : com ( com ), id ( i ) {}

    unsigned long com;
    unsigned long id;
};


struct Cells {
private:
    Boundary* b;
    std::shared_ptr<Parameters> p;
    std::vector<unsigned int> DensityVector;
    std::vector<Cell> cells;
    int sz;
    unsigned int NumberOfCells = 0;
    bool init_flag = false;
    
    struct IsCOM {
        IsCOM ( unsigned int x_ ) : x ( x_ ) {}
        unsigned int x;
        bool operator() ( Cell c ) {
            return c.com==x;
        }
    };
    
    void computeDensity() {
        for ( auto i : range<unsigned> ( 0, p->DomainSizeL ) ) {
            DensityVector[i] = countCOM ( i );
        }
    }

    unsigned long countCOM ( int x ) {
        return std::count_if ( cells.begin(), cells.end(), IsCOM ( x ) );
    }

    void reinit() {
            tearDown();
            init();        
    }
   
    void init() {
        int density = std::max(1UL, p->NumberOfCells / ( p->DomainSizeL ) );
        DEBUG2(density);
        
	// update NumberOfCells to a number that fits with the uniform starting arrangement
	p->NumberOfCells = p->DomainSizeL * density;

        cells.reserve(p->NumberOfCells);
	std::cout << "Creating " << p->NumberOfCells << " cells." << std::endl;

        for ( auto i : range<unsigned>(0, p->DomainSizeL) )
        	repeat(density, [i, this] {append(i); } );
        
        b = BoundaryFactory::createBoundary ( BoundaryFactory::periodic );
        DensityVector.resize ( p->DomainSizeL, 0 );
        computeDensity();
        init_flag = true;
    }
    
    void tearDown() {
        b = 0;
        p = 0;
        DensityVector.clear();
        cells.clear();
        init_flag = false;
    }
    
    void append ( double com ) {
        cells.emplace_back ( Cell ( com, NumberOfCells++ ) );
    }
     
public:  
    Cells() : b(0), p(0), sz(0) 
    {}
    
    Cells ( std::shared_ptr<Parameters>  p_ ) : b(0), p ( p_ ), sz(0) {
        init();
    }

    void update(std::shared_ptr<Parameters> p_)
    {
        init_flag = false;
        p = p_;
        reinit();
    }
    
    bool checkInit() { return init_flag; }
    bool forceInit() { init(); return init_flag; } 
     
    Cell& getCell ( unsigned int id ) {
        ASSERT ( id<=cells.size(), "Id " << id << " is not valid! Cell id has to be in the interval [0," << cells.size()-1 << "]." );
        return cells[id];
    }

    void updatePosition ( Cell& cell, short increment ) {
        ASSERT(init_flag, "Cells has to be initialized first!");
        ASSERT( increment==-1 || increment==1, "increment can only be +1 or -1");
        
        if ( DensityVector[cell.com] != 0 ) {
            DensityVector[cell.com]-=1;
        }

        int NewPos = static_cast<int>(cell.com) + increment;
        b->applyCondition (NewPos, p->DomainSizeL );
        ASSERT ( VALID_LATTICE ( NewPos ), "NewCellCOM" << NewPos << " invalid lattice position!" );
      
        //DEBUG2(NewPos);
        cell.com = NewPos;
        DensityVector[cell.com]+=1;
    }

    void updatePosition ( unsigned int id, short increment ) {
        ASSERT ( id>=0 && id<=cells.size(), "" );
        ASSERT ( cells[id].id==id, "id was " << id << " cellid was " << cells[id].id );
        updatePosition ( cells[id], increment );
    }

    std::shared_ptr<Parameters> getParameters() {
        return p;
    }

    void SetParameters ( std::shared_ptr<Parameters> _p ) {
        p = _p;
        reinit();
    }
    
    void print() {
        for ( auto p : cells ) {
            std::cout << "ID=" << p.id << " COM="<< p.com << std::endl;
        }
    }

    void print ( int c ) {
        std::cout << "ID=" << cells[c].id << " COM="<<cells[c].com << std::endl;
    }

    unsigned int getDensity ( int x ) {
        ASSERT(init_flag, "Cells has to be initialized first!");        
        //std::cout << "DL=" << p->DomainSizeL <<std::endl;

        //assert(VALID_LATTICE(b->applyCondition(x, p->DomainSizeL));

        //assert(b->applyCondition(x, p->DomainSizeL) >=0 && b->applyCondition(x, p->DomainSizeL)<=p->DomainSizeL  );
        //std::cout << "x="<<x<<std::endl;
        b->applyCondition ( x, p->DomainSizeL );
        //std::cout << "x="<<x<<std::endl;
        //std::cout << "sz="<<DensityVector.size()<<std::endl;
        ASSERT ((unsigned)x<DensityVector.size(), "x "<< x << " invalid density vector index!");
        return DensityVector[x];
    }
    
    std::vector<unsigned int> getDensityVector()
    {
        return DensityVector;
    }
    
    std::vector<double> getCellCOM()
    {
        std::vector<double> ret;
        ret.reserve(cells.size());
        
        for (const auto& cell : cells)
        {
            ret.push_back(cell.com * p->StepSize);
        }
        return ret;
    }

};

#endif
