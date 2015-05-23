#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include "debug.h"

class Parameters
{
public:
    Parameters() {}
        Parameters(double Dsize_, double StepSize_,
                   double FinalTime_, unsigned long _NumberOfCells)
        : DomainSize(Dsize_), DomainSizeL(),
        StepSize(StepSize_), FinalTime(FinalTime_),
        SensingRadius(1.0), SensingRadiusL(0),
        NumberOfCells(_NumberOfCells)
        {
                DomainSizeL = DomainSize / StepSize;
                SensingRadiusL = SensingRadius / StepSize;

                std::cout << "Domain Size:" << DomainSizeL;
                std::cout << " Sensing Radius:" << SensingRadiusL;
                std::cout << " FinalTime:" << FinalTime << std::endl;

                ASSERT(NumberOfCells!=0, "Number of cells can't be zero!");

                Check();
        }

        void Check()
        {
                assert(0.0<=InitCellDensity && InitCellDensity<=1.0);
        }

        const double getFinalTime() const { return FinalTime; }

        const double getDiffusion() const { return Diffusion; }
        const double getDomainSize() const { return DomainSize; }
        const unsigned long getDomainSizeL() const { return DomainSizeL; }
        const double getDiscreteX() const { return StepSize; }
        const unsigned long getNumberOfCells() const { return NumberOfCells; }

private:
        double DomainSize;
        unsigned long DomainSizeL;

        double StepSize;
        double FinalTime;

        double SensingRadius = 1.0;
        unsigned long SensingRadiusL;

        unsigned long NumberOfCells;
        double InitCellDensity = 0.5;

        double Diffusion = 0.5;
        double Adhesion = 0.1;
};


#endif
