
#pragma once

#include <Parameters.h>
#include <memory>
#include <array>
#include <vector>

#include <cassert>

class PropensitiesGenerator
{
	public:
		PropensitiesGenerator() {}

		virtual std::array<double, 2>
			compute(const long coordinate) const = 0;

		virtual PropensitiesGenerator& set(const std::shared_ptr<Parameters>& p) = 0;
};

class DiffusionPropensities : public PropensitiesGenerator
{
	public:
		DiffusionPropensities()	{}

		DiffusionPropensities& set(const std::shared_ptr<Parameters>& p)
		{
			diffusion_coeff = p->getDiffusionSim();
			return *this;
		}

		inline std::array<double, 2>
		compute(const long coordinate) const
		{
			return {diffusion_coeff, diffusion_coeff};
		}

	private:
		double diffusion_coeff;
};

class DriftPropensities : public PropensitiesGenerator
{
	public:
		DriftPropensities() {}

		DriftPropensities& set(const std::shared_ptr<Parameters>& p)
		{
			rhs = p->getDiffusionSim() + p->getDriftSim() * p->getDiscreteX();
			lhs = p->getDiffusionSim() - p->getDriftSim() * p->getDiscreteX();

			assert(rhs>=0.0);
			assert(lhs>=0.0);

			return *this;
		}

		inline std::array<double, 2>
		compute(const long coordinate) const
		{
			return {rhs, lhs};
		}

	private:
		double rhs, lhs;
};

class AdhesionPropensities : public PropensitiesGenerator
{
	public:
		AdhesionPropensities() {}

		AdhesionPropensities& set(const std::shared_ptr<Parameters>& p)
		{
			rhs = p->getDiffusionSim() + p->getDriftSim() * p->getDiscreteX();
			lhs = p->getDiffusionSim() - p->getDriftSim() * p->getDiscreteX();

			assert(rhs>=0.0);
			assert(lhs>=0.0);

			return *this;
		}

		inline std::array<double, 2>
		compute(const long coordinate) const
		{
			return {rhs, lhs};
		}

/*
double RandomWalk::omega ( long coordinate )
{
	return 1.0;
}

double RandomWalk::adhesivity ( long coordinate )
{
	return state->getDensity ( coordinate );
}

double RandomWalk::space ( long coordinate )
{
	return 1.0; //std::max(0.0, 1.0 - (state->get(r) / CarryingCapacity) );
}

double RandomWalk::PolarizationVector ( long coordinate )
{
	// FIXME compute this without jumping around in memory constantly
	double total {0.0};
	for ( auto offset : range<unsigned> ( 0, sensing_offset ) )
		total+= ( space ( coordinate + offset ) * omega ( +offset ) * adhesivity ( coordinate + offset ) -
				space ( coordinate - offset ) * omega ( -offset ) * adhesivity ( coordinate - offset ) );

	return total / TotalNumberOfCells;
}
*/
	private:
		double rhs, lhs;
};


