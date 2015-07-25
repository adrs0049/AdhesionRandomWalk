
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
#include <Simulator_traits.h>

class PropensitiesGenerator
{
	protected:
		using state_vector = const stateVector<int32_t>;
		using state_vector_ptr = const state_vector*;

	public:
		PropensitiesGenerator() {}

		virtual std::array<double, 2>
			compute(const long coordinate) const = 0;

		virtual PropensitiesGenerator& set(const std::shared_ptr<Parameters>& p) = 0;
		virtual PropensitiesGenerator& set(state_vector_ptr) = 0;
		virtual void verify(void) const = 0;
};

class DiffusionPropensities : public PropensitiesGenerator
{
	public:
		DiffusionPropensities()	{}

		DiffusionPropensities& set(const std::shared_ptr<Parameters>& p);
		DiffusionPropensities& set(state_vector_ptr);

		void verify(void) const;

		inline std::array<double, 2>
		compute(const long coordinate) const;

	private:
		double diffusion_coeff;
};

class DriftPropensities : public PropensitiesGenerator
{
	public:
		DriftPropensities() {}

		DriftPropensities& set(const std::shared_ptr<Parameters>& p);
		DriftPropensities& set(state_vector_ptr);

		void verify(void) const;

		inline std::array<double, 2>
		compute(const long coordinate) const;

	private:
		double rhs, lhs;
};

class AdhesionPropensities : public PropensitiesGenerator
{
		using float_type = typename simulation_traits::float_type;
		using vec_type = simd_traits<float_type>::type;
		using vec_type_int = simd_traits<int32_t>::type;
		static const std::size_t vec_size = simd_traits<float_type>::size;
		static const std::size_t vec_size_int = simd_traits<int32_t>::size;

	public:
		AdhesionPropensities() {}

		AdhesionPropensities& set(state_vector_ptr state);
		AdhesionPropensities& set(const std::shared_ptr<Parameters>& p);

		void verify(void) const;

		inline std::array<double, 2>
		compute(const long coordinate) const;

	private:
		double diffusion_coeff;
		double drift_coeff_and_h;
		unsigned long sensing_radius;
		unsigned long TotalNumberOfCells;
		unsigned long domainSizeL;

		double omega_normalization_constant;

		// TODO better solns? readonly shared ptr?
		//std::shared_ptr<state_vector> state;
		state_vector_ptr state;

		std::function<vec_type (const long)> space;
		std::function<vec_type_int (const long)> adhesivity;
		std::function<vec_type (const long)> omega;
};


