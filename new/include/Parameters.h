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
            std::cout << "Domain Size:" << DomainSizeL;
            std::cout << " Sensing Radius:" << SensingRadiusL;
            std::cout << " FinalTime:" << FinalTime << std::endl;

            update();
        }

        void update()
        {
            DomainSizeL = DomainSize / StepSize;
            SensingRadiusL = SensingRadius / StepSize;
            lambda = 2.0 * Diffusion;

            Check();
        }

        void Check()
        {
            ASSERT(NumberOfCells!=0, "Number of cells can't be zero!");
            ASSERT(0.0<=InitCellDensity && InitCellDensity<=1.0, "Initial Cell Density " << InitCellDensity << " is invalid. The valid range is [0,1].");
            ASSERT(Diffusion>=0.0, "The diffusion coefficient can't be negative");
        }

        const double getFinalTime() const { return FinalTime; }

        const double getDiffusion() const { return Diffusion; }
        const double getDomainSize() const { return DomainSize; }
        const unsigned long getDomainSizeL() const { return DomainSizeL; }
        const double getSensingRadius() const { return SensingRadius; }
        const unsigned long getSensingRadiusL() const { return SensingRadiusL; }
        const double getDiscreteX() const { return StepSize; }
        const unsigned long getNumberOfCells() const { return NumberOfCells; }
        const double getLambda() const { return lambda; }
        const double getStepSizeSquare() const { return StepSize * StepSize; }
        const double getDiffusionSim() const { return  getDiffusion() / ( getLambda() * getStepSizeSquare()); }
        const double getDrift() const { return Drift; }
        const double getDriftSim() const { return getDrift() / ( 2 * getLambda() * getStepSizeSquare()); }

        void setDiffusion(double _Diffusion) { Diffusion = _Diffusion; }
        void setDrift(double _Drift) { Drift = _Drift; }

private:
        double DomainSize;
        unsigned long DomainSizeL;

        double StepSize;
        double FinalTime;

        // see random jump derivation
        double lambda;

        double SensingRadius = 1.0;
        unsigned long SensingRadiusL;

        unsigned long NumberOfCells;
        double InitCellDensity = 0.5;

        double Diffusion = 0.5;
        double Drift = 0.0;
};


#endif
