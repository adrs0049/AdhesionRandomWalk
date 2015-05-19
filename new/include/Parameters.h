#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <cassert>

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

                Check();
        }

        void Check()
        {
                assert(0.0<=InitCellDensity && InitCellDensity<=1.0);
        }

        double getFinalTime() { return FinalTime; }

        double getDiffusion() { return Diffusion; }
        double getDomainSize() { return DomainSize; }
        unsigned long getDomainSizeL() { return DomainSizeL; }
        double getDiscreteX() { return StepSize; }
        unsigned long getNumberOfCells() { return NumberOfCells; }

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
