#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "Boundary.h"

enum class RANDOMWALK_TYPE : int
{
	DIFFUSION, // const diffusion
	DIFFUSION_AND_DRIFT, // constant diffusion + drift
	ADHESION, // adhesion
};

enum class IC_TYPE : int
{
	UNIFORM,
	DELTA,
	RANDOM,
	HEAVISIDE_RIGHT,
	HEAVISIDE_LEFT,
	TRIG_NOISE,
};

enum class OMEGA_TYPE : int
{
	UNIFORM,
	TRIANGLE,
};

enum class SPACE_TYPE : int
{
	ALWAYS_FREE,
	CLASSICAL_VOLUME_FILLING,
};

enum class ADHESIVITY_TYPE : int
{
	SIMPLE,
};

using namespace boundary;

class Parameters
{
	public:
		Parameters();
		Parameters(double Dsize_, unsigned int DomainN_,
				double FinalTime_);

        Parameters(double Dsize_, unsigned int DomainN_,
				std::vector<double> FinalTimes_);

		Parameters(std::vector<double> _shape, unsigned int _DomainN,
				double _finalTime);

		Parameters(std::vector<double> _shape, unsigned int _DomainN,
				std::vector<double> FinalTimes_);

		void print_info();
		void update();
		void Check();

		const std::vector<double>& getFinalTimes() const { return FinalTimes; }

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
		const unsigned long getSteps() const { return steps; }
		const double getCurrentTime() const { return time; }
		const IC_TYPE& getIcType() const { return ic_type; }
		const OMEGA_TYPE& getOmegaType() const { return omega_type; }
		const double getOmegaP() const { return omega_p; }
		const double getICweight() const { return ic_weight; }
		const BOUNDARY_TYPE& getBoundaryType() const { return boundary_type; }
		const RANDOMWALK_TYPE& getRandomWalkType() const { return rw_type; }
		const SPACE_TYPE& getSpaceType() const { return space_type; }
		const ADHESIVITY_TYPE& getAdhesivityType() const
			{ return adhesivity_type; }

		void setDiffusion(double _Diffusion) { Diffusion = _Diffusion; }
		void setNumberOfCells(long number) { NumberOfCells = number; }
		void setDrift(double _Drift) { Drift = _Drift; }
		void setSensingRadius(double _R) { SensingRadius = _R; }
		void setDiscreteX(double _h) { StepSize = _h; }
		void setLambda(double _l) { lambda = _l; }
		void setSteps(unsigned long _steps) { steps = _steps; }
		void setCurrentTime(const double _time) { time = _time; }
		void setIcP(const unsigned long _ic_p) { ic_p = _ic_p; }
		void setIcType(const IC_TYPE& type) { ic_type = type; }
		void setOmegaType(const OMEGA_TYPE& type) { omega_type = type; }
		void setOmegaP(const double p) { omega_p = p; }
		void setBoundaryType(const BOUNDARY_TYPE& type)
			{ boundary_type = type; }
		void setRandomWalkType(const RANDOMWALK_TYPE& type)
			{ rw_type = type; }
		void setSpaceType(const SPACE_TYPE& type)
			{ space_type = type; }
		void setAdhesivityType(const ADHESIVITY_TYPE& type)
			{ adhesivity_type = type; }
		void setICweight(const double w)
			{ ic_weight = w; }
		// TODO add high dimensions if needed
		std::vector<double> getDomainShape() const { return domainShape; }
		void toggleBase2(void) { base2 = !base2; }

	private:
		bool set = false;

		double DomainSize;
		unsigned long DomainSizeL;

		std::vector<double> domainShape;

		unsigned int DomainN;
		double StepSize;
		std::vector<double> FinalTimes;

		// see random jump derivation
		double lambda = 1E2;

		double SensingRadius = 1.0;
		unsigned long SensingRadiusL;

		unsigned long NumberOfCells;
		double InitCellDensity = 0.5;

		double Diffusion = 0.5;
		double Drift = 0.0;

		// ic density
		unsigned long ic_p = 0;
		IC_TYPE ic_type;
		double ic_weight = 0.1;

		// data for omega function
		double omega_p = 0.0;
		OMEGA_TYPE omega_type = OMEGA_TYPE::UNIFORM;

		// Boundary Type
		BOUNDARY_TYPE boundary_type = BOUNDARY_TYPE::PERIODIC;

		// RandomWalk type
		RANDOMWALK_TYPE rw_type = RANDOMWALK_TYPE::DIFFUSION;

		// Space type
		SPACE_TYPE space_type = SPACE_TYPE::ALWAYS_FREE;

		// Adhesivity type
		ADHESIVITY_TYPE adhesivity_type = ADHESIVITY_TYPE::SIMPLE;

		unsigned long steps = 0;
		double time = 0.0;

		// use base 2 lengths
		bool base2 = false;
};

#endif
