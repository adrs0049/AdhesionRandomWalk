// RandomWalk.cpp

#include "RandomWalk.h"

#include <iostream>
#include <iterator>
#include <exception>
#include <cmath>

#include <Loop.h>
#include <chronos.h>

#include "debug.h"

RandomWalk::~RandomWalk() {}
RandomWalk::RandomWalk() {}
RandomWalk::RandomWalk(std::shared_ptr<Parameters> _param)
    : state(std::make_shared<state_vector>(_param)),
    param(_param),
    rand(0.0, 1.0)
{}

void RandomWalk::GeneratePath()
{
	try {

		// state variables of simulation
		time = 0.0;
		steps=0;

		//print_info();

		//std::cout << "Setup simulation..." << std::endl;

		// ensure all things are set and updated
		setup();

		//std::cout << "Starting timer..." << std::endl;
		chronos::Chronos timer;

		//std::cout << "Starting simulation..." << std::endl;

		// initialize all propensities
		computeAllPropensities();

		ASSERT(finalTimes.size()>0, "FinalTimes is empty!");

		for (auto& finalTime : finalTimes)
		{
			//std::cout << "Current final time is " << finalTime << std::endl;
			do {
				steps++;

				Step();

				//std::cout << "\rTime:" << std::setw(15) << time
				//	<< " Step: " << std::setw(10) << steps;

			} while (time < finalTime );

			param->setSteps(steps);
			param->setCurrentTime(time);

			// TODO signal somehow that data should be written
		    sim->notify(EventType::new_data);
		}

		//std::cout << std::endl;
		//std::cout << "Simulation complete. The total number of steps is " << steps << std::endl;

		//print_info();

	} catch (const std::exception& exception) {
		std::cerr << "ERROR: " << exception.what() << std::endl;
	}
}

void RandomWalk::Step()
{
	auto r1 = rand();
	auto r2 = rand();

	//std::cout << "r1: " << r1 << " r2: " << r2 << std::endl;

	// compute propensity
	double a0 = getPropensitySum();
	ASSERT(a0!=0.0, "total propensity is zero. Error!");

	//std::cout << "a0: " << a0 << std::endl;

	time += 1.0 / a0 * std::log(1.0 / r1);

	// start at -1
	long k {-1};
	double ss {0.0};
	auto r2a0 = r2 * a0;

	//std::cout << "step: " << steps << " r2a0:" << r2a0
	//          << std::endl;

	// propensity stride is all the reactions to the right
	while (ss <= r2a0 && k < propensity_stride - 1)
	{
		k++;
		ss+=getPropensity(k, 0); // d * c->get(k);
	}

	//std::cout << "sum: "<<ss << std::endl;
	if (ss > r2a0)
	{
		// Move to the right
		state->RightShift(k);

	} else {
		// Hit the maximum number in counting
		// Now find a LeftShift reaction
		k=-1; // we also have propensity_stride left rxns now
		while (ss < r2a0 && k < propensity_stride - 1)
		{
			k++;
			ss+=getPropensity(k, 1); // d * c->get(k);
		}

		// The simulation has to find a Rxn
		ASSERT((unsigned)k<param->getDomainSizeL(), "Simulation failed did not find a Rxn to update!");

		state->LeftShift(k);
	}

	//std::cout<<" k: "<<k<<" ss: "<< ss<< " a0="<< a0 <<std::endl;
	// shift is done now update the propensity vector
	updatePropensity(k);
	//computeAllPropensities();

	// Tolerance to account for slight rounding errors
	ASSERT(ss<=a0+1E-3, "Propensity sum (" << ss << ") can't be larger than the propensity sum a0 (" << a0 << ") !");

}

void RandomWalk::setup()
{
	// propensity_stride = -2 for no flux bc
	propensity_stride = param->getDomainSizeL();
	// FIXME to the selection for different boundary conditions better
	// for no flux boundary conditions
	// NumberOfReactions = 2 * (param->getDomainSizeL() - 1);
	// for periodic boundary conditions
	TotalNumberOfCells = param->getNumberOfCells();
	NumberOfReactions = 2 * param->getDomainSizeL();
	sensing_offset = param->getSensingRadiusL();

	propensities.clear();
	propensities.resize(NumberOfReactions, 0.0);

	// get final times
	finalTimes = param->getFinalTimes();

	//std::cout << "NumberOfReactions= " << NumberOfReactions
	//	<< " propensity_stride=" << propensity_stride << std::endl;

	//state->print();
}

void RandomWalk::print_info()
{
	std::cout << "Starting Gillespie's SSA for a simulation a space-jump "
		<< "process. The space-jump process includes diffusion and "
		<< "drift." << std::endl;

	std::cout << "This simulation is run with " << param->getNumberOfCells()
		<< " cells. The final times of the simulation are ";
		std::copy(param->getFinalTimes().begin(),
				  param->getFinalTimes().end(),
				  std::ostream_iterator<double>(std::cout, ", "));
		std::cout << std::endl;

	std::cout << "Courant Number=" << 1.0 / param->getDiffusionSim()
		<< " StepSize=" << param->getDiscreteX()
		<< " Lambda=" << param->getLambda()
		<< " Diffusion=" << param->getDiffusion()
		<< " Drift=" << param->getDrift() << std::endl
		<< " Drift_Sim=" << param->getDriftSim()
		<< " DiffusionRate=" << param->getDiffusionSim()
		<< " DriftRate=" << param->getDriftSim() << std::endl
		<< " SensingRadius=" << param->getSensingRadius()
		<< " SensingRadiusL=" << param->getSensingRadiusL()
		<< std::endl;
}

void  RandomWalk::computeAllPropensities()
{
	for (long k = 0; k < propensity_stride; k++)
		computePropensity(k);
}

bool  RandomWalk::propensityApplyCondition(long& idx, const long& max_value)
{
	if (idx < 0)
	{
		auto val = std::abs((float)(idx % max_value));
		idx = max_value - val;
		return true;
	}
	else if (idx >= max_value)
	{
		idx = idx % max_value;
		return true;
	}
	return false;
}

// TODO write a test to test if this really works
void  RandomWalk::updatePropensity(long coordinate)
{
	// add 1 to the offset
	long offset = sensing_offset + 1;

	for (long k = coordinate - offset; k < coordinate + offset; k++)
	{
		//std::cout << "prop update k="<< k<<std::endl;
		// cant pass k to it otherwise it will be changed!!
		long i {k};
		propensityApplyCondition(i, param->getDomainSizeL());
		computePropensity(i);
	}
}

double RandomWalk::getPropensity(int coordinate, int flag) const
{
	/*
	ASSERT(coordinate >= 0 && (unsigned)coordinate < param->getDomainSizeL(), \
			"Lattice coordinate= " << coordinate << " is invalid. Valid \
			lattice coordinate range is [" << 0 << ", " << \
			param->getDomainSizeL() << ").");

	ASSERT((flag * propensity_stride + coordinate) >=0 \
			&& (unsigned)(flag * propensity_stride + coordinate)<propensities.size(), \
			"RxnIndex=" << flag * propensity_stride + coordinate
			<< " is invalid. The valid range is [" << 0 << ", "
			<< propensities.size() << "). Coordinate=" << coordinate
			<< " flag=" << flag << ".");
	*/
	/*
	   std::cout << "propensity ( " << coordinate << " , " << flag << " ):"
	   << propensities.at(flag * propensity_stride + coordinate) <<
	   " state ( " << coordinate << ")=" <<
	   state->get(coordinate) << std::endl;
	   */

	// TODO save the complete propensity in a vector
	// so we dont have to recompute it each
	//ASSERT(propensities.at(flag * propensity_stride + coordinate)>=0.0,
	//		"Propensity at index " << flag * propensity_stride + coordinate <<
	//		" is negative. (" << propensities.at(flag * propensity_stride + coordinate) \
	//		<< ") Propensities have to be non-negative");

	// TODO check that this offset is correct
	return propensities[flag * propensity_stride + coordinate];
	//return propensities.at(flag * propensity_stride + coordinate);
}

double RandomWalk::getPropensitySum() const
{
	return std::accumulate(propensities.begin(), propensities.end(), 0.0);
}

void RandomWalk::computePropensity( long coordinate )
{
	// compute polarization at the location only once
	auto polarization = PolarizationVector(coordinate);

	// compute components for transition rates
	double symmetric = param->getDiffusionSim();
	double asymmetric = param->getDriftSim() * param->getDiscreteX() * polarization;

	//std::cout << "right=" << (symmetric + asymmetric);
	//std::cout << "left=" << (symmetric - asymmetric) << std::endl;

	// TODO raise an exception!! instead
	assert((symmetric+asymmetric)>=0.0);
	assert((symmetric-asymmetric)>=0.0);

	/*
	ASSERT((symmetric+asymmetric)>=0.0, "tmp =" << symmetric+asymmetric << " "\
			<< " pol=" << polarization << " symm=" << symmetric << " asym="\
			<< asymmetric << " diffusion=" << param->getDiffusionSim() << \
			" drift=" << param->getDriftSim() << \
			" step=" << param->getDiscreteX());

	ASSERT((symmetric-asymmetric)>=0.0, "tmp =" << symmetric-asymmetric << " "\
			<< " pol=" << polarization << " symm=" << symmetric << " asym="\
			<< asymmetric << " diffusion=" << param->getDiffusionSim() << \
			" drift=" << param->getDriftSim() << \
			" step=" << param->getDiscreteX());
	*/

	propensities.at(coordinate) = param->getLambda() *
		state->getDensity(coordinate) * (symmetric + asymmetric);

	propensities.at(propensity_stride + coordinate) = param->getLambda() *
		state->getDensity(coordinate) * (symmetric - asymmetric);
}

std::array<double, 2> RandomWalk::getPropensity( long coordinate )
{
	return { propensities.at(coordinate),
				propensities.at(propensity_stride + coordinate)};
}

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
