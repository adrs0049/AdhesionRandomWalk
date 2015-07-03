#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <vector>
#include "debug.h"

class Parameters
{
	public:
		Parameters() {}
		Parameters(double Dsize_, double StepSize_,
				double FinalTime_, unsigned long _ic_p)
			: DomainSize(Dsize_), DomainSizeL(0),
			StepSize(StepSize_), FinalTime(FinalTime_),
			ic_p(_ic_p)
	{
		// TODO check if this is correct
		domainShape.resize(2, 0);
		domainShape[0] = -DomainSize/2;
		domainShape[1] = DomainSize/2;

		NumberOfCells = 0;

		update();
		print_info();
	}

		Parameters(std::vector<double> _shape, double _stepSize,
				double _finalTime,
				unsigned long _ic_p)
			: domainShape(_shape), StepSize(_stepSize), FinalTime(_finalTime),
			ic_p(_ic_p)
	{
		ASSERT(_shape.size()==2, "shape size is invalid!");

		DomainSize = domainShape[1] - domainShape[0];

		update();
		set = true;
		print_info();
	}

		void print_info()
		{
			Check();
			std::cout << "Domain Size:" << DomainSize;
			std::cout << " Domain Size L:" << DomainSizeL;
			std::cout << " Domain = (" << domainShape[0] << ", "
				<< domainShape[1] << ").";
			std::cout << " StepSize:" << StepSize << std::endl;
			std::cout << " Sensing Radius:" << SensingRadiusL;
			std::cout << " FinalTime:" << FinalTime << std::endl;
		}

		void update()
		{
			DomainSizeL = DomainSize / StepSize;
			SensingRadiusL = SensingRadius / StepSize;
			lambda = 1E2;
			set = true;

			Check();
		}

		void Check()
		{
			ASSERT(set, "Parameters are not initalized.");
			ASSERT(domainShape.size()==2, "Domain shape has invalid size!");

			ASSERT(SensingRadius < DomainSize/2, "SensingRadius is too large!");

			ASSERT(ic_p!=0, "Initial number of cells can't be zero!");
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
		// TODO check why I don't need a lambda here?
		const double getDiffusionSim() const
		{ return getDiffusion() / (getLambda() * getStepSizeSquare()); }
		const double getDrift() const { return Drift; }
		const double getDriftSim() const
		{ return getDrift() / ( 2.0 * getLambda() * getStepSizeSquare()); }
		const unsigned long getICp() { return ic_p; }
		unsigned long getSteps() { return steps; }

		void setDiffusion(double _Diffusion) { Diffusion = _Diffusion; }
		void setNumberOfCells(long number) { NumberOfCells = number; }
		void setDrift(double _Drift) { Drift = _Drift; }
		void setSensingRadius(double _R) { SensingRadius = _R; }
		void setDiscreteX(double _h) { StepSize = _h; }
		void setLambda(double _l) { lambda = _l; }
		void setSteps(unsigned long _steps) { steps = _steps; }

		// TODO add high dimensions if needed
		std::vector<double> getDomainShape() { return domainShape; }

	private:
		bool set = false;

		double DomainSize;
		unsigned long DomainSizeL;

		std::vector<double> domainShape;

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

		unsigned long ic_p = 0;
		unsigned long steps = 0;
};


#endif
