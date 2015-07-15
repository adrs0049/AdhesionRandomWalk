
#pragma once

#include "exceptions.h"
#include "Propensities.h"
#include "Parameters.h"
#include <memory>
#include <make_unique.h>

class PropensitiesGeneratorFactory
{
	public:
		static std::unique_ptr<PropensitiesGenerator>
			createPropensitiesGenerator(const RANDOMWALK_TYPE& rw_type)
		{
			switch (rw_type)
			{
				case RANDOMWALK_TYPE::DIFFUSION:
					return std::make_unique<DiffusionPropensities>();
					break;
				case RANDOMWALK_TYPE::DIFFUSION_AND_DRIFT:
					return std::make_unique<DriftPropensities>();
					break;
				case RANDOMWALK_TYPE::ADHESION:
					return std::make_unique<AdhesionPropensities>();
					break;
				default:
					throw NotImplementedException {""};
			};
			return nullptr;
		}
};
