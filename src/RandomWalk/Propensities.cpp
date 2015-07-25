
#include <simd.hpp>
#include <Propensities.h>

DiffusionPropensities& DiffusionPropensities::set(const std::shared_ptr<Parameters>& p)
{
	diffusion_coeff = p->getDiffusionSim();
	return *this;
}

DiffusionPropensities& DiffusionPropensities::set(state_vector_ptr state)
{
	return *this;
}

void DiffusionPropensities::verify(void) const
{
	ASSERT(diffusion_coeff>0.0, "Diffusion coefficient is "
			<< diffusion_coeff << " but it has to be larger than 0.0!");
}

inline std::array<double, 2>
DiffusionPropensities::compute(const long coordinate) const
{
	return {diffusion_coeff, diffusion_coeff};
}

DriftPropensities& DriftPropensities::set(const std::shared_ptr<Parameters>& p)
{
	rhs = p->getDiffusionSim() + p->getDriftSim() * p->getDiscreteX();
	lhs = p->getDiffusionSim() - p->getDriftSim() * p->getDiscreteX();

	return *this;
}

DriftPropensities& DriftPropensities::set(state_vector_ptr state)
{
	return *this;
}

void DriftPropensities::verify(void) const
{
	ASSERT(rhs>0.0, "Right hand side propensity is "
			<< rhs << " but it has to be larger than 0.0!");

	ASSERT(lhs>0.0, "Left hand side propensity is "
			<< lhs << " but it has to be larger than 0.0!");
}

inline std::array<double, 2>
DriftPropensities::compute(const long coordinate) const
{
	return {rhs, lhs};
}

AdhesionPropensities& AdhesionPropensities::set(state_vector_ptr state_ptr)
{
	state = state_ptr;
	return *this;
}

AdhesionPropensities& AdhesionPropensities::set(const std::shared_ptr<Parameters>& p)
{
	diffusion_coeff = p->getDiffusionSim();
	drift_coeff_and_h = p->getDriftSim() * p->getDiscreteX();
	sensing_radius = p->getSensingRadiusL();
	TotalNumberOfCells = p->getNumberOfCells();
	domainSizeL = p->getDomainSizeL();

	const auto& omega_type = p->getOmegaType();
	const auto& space_type = p->getSpaceType();
	const auto& adhesivity_type = p->getAdhesivityType();

	switch (omega_type)
	{
		case OMEGA_TYPE::UNIFORM:
			omega_normalization_constant =
				p->getOmegaP() / (2.0 * p->getSensingRadius());

			omega = [this] (const long coordinate)
			{
				return vec_type(omega_normalization_constant);
			};

			break;

		case OMEGA_TYPE::TRIANGLE:
			throw NotImplementedException {"OMEGA_TYPE::TRIANGLE"};
			break;

		default:
			throw NotImplementedException {"OMEGA_TYPE UNKNOWN"};
			break;
	};

	switch (space_type)
	{
		case SPACE_TYPE::ALWAYS_FREE:
			space = [this] (const long coordinate)
			{
				// TODO check if static helps here
				return vec_type(1.0);
			};

			break;

		case SPACE_TYPE::CLASSICAL_VOLUME_FILLING:
			throw NotImplementedException
			{"SPACE_TYPE::CLASSICAL_VOLUME_FILLING"};
			break;

		default:
			throw NotImplementedException {"SPACE_TYPE UNKNOWN"};
			break;
	};

	switch (adhesivity_type)
	{
		case ADHESIVITY_TYPE::SIMPLE:
			// loads the values to the right of coordinate
			adhesivity = [this] (int coordinate)
			{
				state->applyCondition(coordinate);
				// check if this static cast casuses problems
				if (coordinate < static_cast<long>(domainSizeL - vec_size_int))
				{
					vector8i ret;
					ret.load_u(state->data(coordinate));
					return ret;
				}
				else
				{
					int32_t x[vec_size_int];
					for (std::size_t idx = 0; idx < vec_size_int; idx++)
					{
						x[idx] = state->getDensityQuick(coordinate + idx);
					}
					vec_type_int ret = load_u(x);
					return ret;
				}
			};

			break;

		default:
			throw NotImplementedException {"ADHESIVITY_TYPE UNKNOWN"};
			break;
	};

	return *this;
}

void AdhesionPropensities::verify(void) const
{
	ASSERT(diffusion_coeff>0.0, "Diffusion coefficient is " <<
			diffusion_coeff << " but it has to be larger than 0.0!");

	ASSERT(drift_coeff_and_h>0.0, "Drift coefficient and StepSize is "
			<< drift_coeff_and_h << " but it has to be larger than 0.0!");

	ASSERT(sensing_radius>0, "The sensing radius is " << sensing_radius
			<< " but it has be larger than 0!");

	ASSERT(TotalNumberOfCells>0, "The Total Number of cells is "
			<< TotalNumberOfCells << " but it has to be larger than 0!");

	const unsigned int CellLimit = 1E9;
	ASSERT(TotalNumberOfCells<CellLimit, "The Total Number of cells is "
			<< TotalNumberOfCells << " ie. larger than "
			<< CellLimit << " make sure this is intended!");

	ASSERT(omega_normalization_constant>0.0, "Omega normalization constant is "
			<< omega_normalization_constant << " but it has be larger than 0.0!");

	const double omegaLimit = 1E9;
	ASSERT(omega_normalization_constant<omegaLimit, "Omega normalization constant is "
			<< omega_normalization_constant << " which is larger than "
			<< omegaLimit << " make sure this intended!");

	ASSERT(sensing_radius % vec_size == 0, "sensing radius has to be a multiple of vec_size!");

	if (!state) throw NullPtrDereference {"state_ptr"};
}

inline std::array<double, 2>
AdhesionPropensities::compute(const long coordinate) const
{
	// compute polarization
	double total {0.0};
	const std::size_t regularpart = sensing_radius & (-vec_size_int);
	assert(regularpart == sensing_radius);

	//std::cout <<"regular part=" << regularpart <<
	//	" coordinate=" << coordinate << std::endl;
	// start at offset 1
	int32_t i = 0;
	vec_type_int sum1(0);
	// TODO write as integer additions
	for (i = vec_size_int; i <= (int)regularpart; i += vec_size_int)
	{
		//vec_type rv = space(-i) * omega(-i) * adhesivity(coordinate - i);
		vec_type_int rv = adhesivity(coordinate - i);
		//std::cout << "i=" << i<<" "<<" coord-i="<<coordinate -i<<" "<<
		//	"rv=(" << rv[0] << ", " <<rv[1] <<", "<<rv[2]<<", "
		//	<<rv[3]<<")."<<std::endl;
		sum1 -= rv;
	}

	total += hadd(sum1);

	std::size_t j = 1;
	vec_type_int sum2(0);
	for (;j < regularpart; j += vec_size_int)
	{
		//vec_type rv = space(j) * omega(j) * adhesivity(coordinate + j);
		vec_type_int rv = adhesivity(coordinate + j);
		sum2 += rv;
	}

	total += hadd(sum2);
	total *= omega_normalization_constant;

	//total /= TotalNumberOfCells;
	//std::cout << "diffusion_coeff=" << diffusion_coeff << " " <<
 	//	"total=" << total << " drift=" << drift_coeff_and_h << "."
	//	<<std::endl;
	total *= drift_coeff_and_h;

	return {diffusion_coeff + total,
		diffusion_coeff - total};
}
