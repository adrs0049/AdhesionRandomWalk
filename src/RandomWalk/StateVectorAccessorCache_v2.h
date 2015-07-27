
#pragma once

#include <cstdint>
#include <vector>
#include <flat_index_map.h>
#include <Boundary.h>
#include <BoundaryFactory.h>
#include <Parameters.h>
#include <memory>
#include <cassert>
#include <utility>

using namespace boundary;

class StateVectorAccessorCache_v2
{
	using index_type = int32_t;
	using offset_type = std::vector<int32_t>;
	using BoundaryPtr = Boundary*;
	using ParameterPtr = std::shared_ptr<Parameters>;

	public:
		StateVectorAccessorCache_v2()
		: SensingRadiusL(0), domainSizeL(0)
		{}

		StateVectorAccessorCache_v2(ParameterPtr p)
			: SensingRadiusL(p->getSensingRadiusL()),
			domainSizeL(p->getDomainSizeL()),
			m_cache(0)
		{
			std::cout << "accessor size=" << size() << std::endl;
			m_cache.resize(size());
			std::cout << "cache size=" << m_cache.size() << std::endl;
		}

		void setup(ParameterPtr p)
		{
			b = BoundaryFactory::createBoundary ( p->getBoundaryType() );
			//std::cout << "SensingRadiusL=" << SensingRadiusL << std::endl;
			//std::cout << "compute" << std::endl;
			compute();
		}

		inline index_type operator() (const index_type coordinate, const index_type offset)
		{
			return *(m_cache.at(vector_offset(coordinate)).data() + vector_offset(offset));
		}

		inline const index_type operator() (const index_type coordinate, const index_type offset) const
		{
			return *(m_cache.at(vector_offset(coordinate)).data() + vector_offset(offset));
		}

	private:

		const index_type begin() const { return - SensingRadiusL; }
		const index_type end() const { return domainSizeL + SensingRadiusL + 1; }
		const index_type sensing_begin() const { return - SensingRadiusL; }
		const index_type sensing_end() const { return SensingRadiusL + 1; }
		const std::size_t offset_vector_size() const { return 2*SensingRadiusL+1; }
		const std::size_t size() const { return 2 * SensingRadiusL + 1 + domainSizeL; }

		const index_type vector_offset(const index_type idx) const
		{
			return idx + SensingRadiusL;
		}

		void compute()
		{
			for (int coordinate = begin(); coordinate < end(); coordinate++)
			{
				//std::cout << "coordinate=" << coordinate;
				//std::cout << " SensingRadius=" <<SensingRadiusL;
				//std::cout << " DomainSize=" << domainSizeL<<std::endl;
				// for each coordinate compute the offset values of one
				// sensing radius

				std::vector<index_type> offset_vector;
				offset_vector.resize(offset_vector_size(), 0);
				//std::cout << "offset vector resized to " << offset_vector_size()  <<" ";
				//std::cout << "idx begin=" << sensing_begin() << " sensing end=" << sensing_end() << std::endl;

				for (int idx = sensing_begin(); idx < sensing_end(); idx++)
				{
					//std::cout << "sensing_rad=" << SensingRadiusL
					//	<< " correct idx = " << idx << " max=" << sensing_end();

					int tmp = coordinate + idx;
					b->applyCondition(tmp, domainSizeL);
					auto vidx = vector_offset(idx);
					assert(vidx>=0);
					//std::cout << " Storing in offset_vector[" << vidx << "]." << std::endl;
					offset_vector.at(vidx) = tmp;
				}
				//std::cout << "emplace back the offset vector" << std::endl;
				m_cache.insert(std::make_pair(vector_offset(coordinate), offset_vector));
			}
		}

		BoundaryPtr b;
		flat_index_map<offset_type, index_type> m_cache;

		const int SensingRadiusL;
		const int domainSizeL;
};


