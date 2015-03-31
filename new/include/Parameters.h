#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <cassert>

struct Parameters 
{
        Parameters() {}
        Parameters(double Dsize_, double StepSize_, 
                   double TimeStep_, double FinalTime_)
        : DomainSize(Dsize_), DomainSizeL(),
        StepSize(StepSize_), TimeStep(TimeStep_), FinalTime(FinalTime_),
        SensingRadius(1.0), SensingRadiusL(0)
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
                
                // HACK ADJUST time step to make it fit
                while ( TimeStep > ( (StepSize * StepSize) / (2.0 * Diffusion) ))
                        TimeStep /= 2.0;    
        }

        double DomainSize;
        unsigned long DomainSizeL;
        
        double StepSize;
        double TimeStep;
        double FinalTime;       

        double SensingRadius = 1.0;
        unsigned long SensingRadiusL;

        unsigned long NumberOfCells = 100; 
        double InitCellDensity = 0.5;

        double Diffusion = 0.5;
        double Adhesion = 0.1;
};


#endif
