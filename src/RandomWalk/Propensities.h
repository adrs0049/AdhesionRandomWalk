
#pragma once

#include <Parameters.h>
#include <memory>
#include <array>
#include <vector>

#include <cassert>
#include <exceptions.h>
#include <debug.h>

#include <StateVector.h>

// FIXME fix all these complicated include structure!
#include <simd.hpp>
#include <simd_traits.h>
#include <simd_traits_avx.h>
#include <vector4d.h>

class PropensitiesGenerator
{
	protected:
		using state_vector = const stateVector<unsigned int>;

	public:
		PropensitiesGenerator() {}

		virtual std::array<double, 2>
			compute(const long coordinate) const = 0;

		virtual PropensitiesGenerator& set(const std::shared_ptr<Parameters>& p) = 0;
		virtual PropensitiesGenerator& set(const std::shared_ptr<state_vector>&) = 0;
		virtual void verify(void) const = 0;
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

		DiffusionPropensities& set(const std::shared_ptr<state_vector>& state)
		{
			return *this;
		}

		void verify(void) const
		{
			ASSERT(diffusion_coeff>0.0, "Diffusion coefficient is "
					<< diffusion_coeff << " but it has to be larger than 0.0!");
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

			return *this;
		}

		DriftPropensities& set(const std::shared_ptr<state_vector>& state)
		{
			return *this;
		}

		void verify(void) const
		{
			ASSERT(rhs>0.0, "Right hand side propensity is "
					<< rhs << " but it has to be larger than 0.0!");

			ASSERT(lhs>0.0, "Left hand side propensity is "
					<< lhs << " but it has to be larger than 0.0!");
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
		using vec_type = simd_traits<double>::type;
		std::size_t vec_size = simd_traits<double>::size;

	public:
		AdhesionPropensities() {}

		AdhesionPropensities& set(const
				std::shared_ptr<state_vector>& state_ptr)
		{
			state = state_ptr;
			return *this;
		}

		AdhesionPropensities& set(const std::shared_ptr<Parameters>& p)
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
						1.0 / (2.0 * p->getSensingRadius());

					omega = [this] (const long coordinate)
					{
						return omega_normalization_constant;
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
						return 1.0;
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
					adhesivity = [this] (const long coordinate)
					{
						return state->getDensityQuick(coordinate);
					};

					break;

				default:
					throw NotImplementedException {"ADHESIVITY_TYPE UNKNOWN"};
					break;
			};

			return *this;
		}

		void verify(void) const
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

			if (!state) throw NullPtrDereference {"state_ptr"};
		}

		inline std::array<double, 2>
		compute(const long coordinate) const
		{
			// compute polarization
			double total {0.0};
			const std::size_t regularpart = sensing_radius & (-vec_size);

			std::size_t i = 0;
			vec_type sum1(0);
			for (;i < regularpart; i += vec_size)
			{
				// FIXME both are assumed to be uniform for the moment
				vec_type space_vec(space(i));
				vec_type omega_vec(omega(i));
				vec_type adhes_vec(adhesivity(coordinate - i),
								   adhesivity(coordinate - (i+1)),
								   adhesivity(coordinate - (i+2)),
								   adhesivity(coordinate - (i+3)));

				vec_type rv = space_vec * omega_vec * adhes_vec;

				sum1 -= rv;
			}

			total += hadd(sum1);

			// loop for the last <4 elements
			// FIXME deal with this differently!!
			for (; i < sensing_radius; ++i)
				total -= space(coordinate - i) *
					omega(-i) * adhesivity(coordinate - i);

			std::size_t j = 0;
			vec_type sum2(0);
			for (;j < regularpart; j += vec_size)
			{
				// FIXME both are assumed to be uniform for the moment
				vec_type space_vec(space(j));
				vec_type omega_vec(omega(j));
				vec_type adhes_vec(adhesivity(coordinate - j),
								   adhesivity(coordinate - (j+1)),
								   adhesivity(coordinate - (j+2)),
								   adhesivity(coordinate - (j+3)));

				vec_type rv = space_vec * omega_vec * adhes_vec;

				sum2 += rv;
			}

			total += hadd(sum2);

			// loop for the last <4 elements
			// FIXME deal with this differently!!
			for (; j < sensing_radius; ++j)
				total += space(coordinate - j) *
					omega(-j) * adhesivity(coordinate - j);

			total /= TotalNumberOfCells;
			total *= drift_coeff_and_h;

			return {diffusion_coeff + total,
					diffusion_coeff - total};
		}

	private:
		double diffusion_coeff;
		double drift_coeff_and_h;
		unsigned long sensing_radius;
		unsigned long TotalNumberOfCells;
		unsigned long domainSizeL;

		double omega_normalization_constant;

		// TODO better solns? readonly shared ptr?
		std::shared_ptr<state_vector> state;

		std::function<double (const long)> space;
		std::function<double (const long)> adhesivity;
		std::function<double (const long)> omega;
};


