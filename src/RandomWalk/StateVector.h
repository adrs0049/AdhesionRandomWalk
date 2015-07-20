#ifndef CELL_DENSITY_H
#define CELL_DENSITY_H

#include "Boundary.h"
#include "BoundaryFactory.h"
#include "PeriodicBoundary.h"
#include "Loop.h"
#include "Parameters.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <memory>
#include <random>

#include "debug.h"
#include <cassert>
#include "vector.h"
#include "exceptions.h"

using namespace boundary;

template<typename T>
class stateVector_impl
{
private:
	using BoundaryPtr = Boundary*;
	using ParameterPtr = std::shared_ptr<Parameters>;
public:
	//using storage_type = typename std::vector<T>;
	using storage_type = AligndVector<T>;
	using size_type = typename storage_type::size_type;

protected:
	storage_type m_stateVector;

private:
	BoundaryPtr b;
    ParameterPtr p;

	// buffer the domain size locally it's used a lot
	// shared ptr access seems expensive is this true??
	size_type domainSize;

	bool init_flag = false;

    void reinit()
    {
            tearDown();
            init();
			update();
    }

    void init()
    {
        b = BoundaryFactory::createBoundary ( p->getBoundaryType() );
        init_flag = true; // FIXME am i still useful?

		auto& ic_type = p->getIcType();

		switch (ic_type)
		{
			case IC_TYPE::UNIFORM:
				init_uniform();
				break;
			case IC_TYPE::DELTA:
				init_delta();
				break;
			case IC_TYPE::RANDOM:
				init_random();
				break;
			case IC_TYPE::HEAVISIDE_LEFT:
			case IC_TYPE::HEAVISIDE_RIGHT:
				init_heaviside(ic_type);
				break;
			case IC_TYPE::TRIG_NOISE:
				init_noise();
				break;
			default:
				throw NotImplementedException{"IC_TYPE UNKNOWN"};
				break;
		}

		update();
    }

    void tearDown()
    {
        b = nullptr;
        p = nullptr;
        m_stateVector.clear();
        init_flag = false;
    }

	void init_noise()
	{
		std::cout << "init heaviside, vector size is " <<
			m_stateVector.size() << std::endl;

		const double coordinate {0.0};
		const double weight {1E-1};
		const double StepSize = p->getDiscreteX();

		for (std::size_t idx = 0; idx < size(); idx++)
		{
			m_stateVector[idx] = p->getICp() +
				static_cast<unsigned long>(weight * p->getICp() *
						std::sin(coordinate + idx * StepSize) );
		}

		p->setNumberOfCells(getTotalNumberOfCells());
	}

	void init_heaviside(const IC_TYPE& type)
	{
		std::cout << "init heaviside, vector size is " <<
			m_stateVector.size() << std::endl;

		// this includes mid point
		auto mid = m_stateVector.size()/2;
		if (type == IC_TYPE::HEAVISIDE_LEFT)
		{
			for (std::size_t idx = 0; idx < mid; idx++)
				m_stateVector[idx] = p->getICp();
		}
		else if (type == IC_TYPE::HEAVISIDE_RIGHT)
		{
			for (std::size_t idx = mid - 1; idx < m_stateVector.size(); idx++)
				m_stateVector[idx] = p->getICp();
		}
		else
		{
			assert(false);
		}
		p->setNumberOfCells(getTotalNumberOfCells());
		print();
	}

	void init_random()
	{
		std::cout << "init random, vector size is " << m_stateVector.size() << std::endl;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, p->getICp());

		for (auto& elem : m_stateVector)
			elem = dis(gen);

		p->setNumberOfCells(getTotalNumberOfCells());

		print();
	}

    void init_uniform()
    {
        std::cout << "init uniform. vector size is " << m_stateVector.size() << std::endl;
		m_stateVector.assign(m_stateVector.size(), p->getICp());
		std::cout << "number of cells = " << getTotalNumberOfCells() << std::endl;
        p->setNumberOfCells(getTotalNumberOfCells());

		print();
    }

	unsigned long getTotalNumberOfCells()
	{
		return std::accumulate(m_stateVector.begin(), m_stateVector.end(), 0);
	}

    void init_delta()
    {
		auto mid = m_stateVector.size()/2;

        std::cout << "init delta at locations ("
			<< mid-2 << ", " << mid-1 << ", " << mid << ")."
			<< " Each with " << p->getICp() << " number of cells."
			<< std::endl;

		// index is zero based
		m_stateVector[mid-2] = p->getICp();
        m_stateVector[mid-1]= p->getICp();
        m_stateVector[mid]= p->getICp();

		p->setNumberOfCells(getTotalNumberOfCells());

		print();
    }

	void update()
	{
		domainSize = p->getDomainSizeL();
	}

public:

	stateVector_impl()
		: m_stateVector(0), p(nullptr), b(nullptr)
	{}

    stateVector_impl(std::shared_ptr<Parameters> p_)
		: m_stateVector(p_->getDomainSizeL(), 0), b(nullptr), p(p_)
    {
        init();
    }

	storage_type getStateVector() const { return m_stateVector; }

 	bool checkTotal(void) const
	{
		return getTotalNumberOfCells() == p->getNumberOfCells();
    }

    void SetParameters(std::shared_ptr<Parameters> _p)
    {
        p = _p;
        reinit();
    }

	void print() const
	{
		std::cout << "stateVector=(";
		std::copy(m_stateVector.begin(),
				  m_stateVector.end(),
				  std::ostream_iterator<T>(std::cout, ", "));
		std::cout << ")" << std::endl;
	}

	storage_type& getState(void) { return m_stateVector; }
	const storage_type& getState(void) const { return m_stateVector; }

	const T& operator[] (int idx) const
	{
		applyCondition(idx);
		return m_stateVector[idx];
	}

	T& operator[] (int idx)
	{
		applyCondition(idx);
		return m_stateVector[idx];
	}

	T& getDensityQuick(int idx)
	{
		applyCondition(idx);
		return m_stateVector[idx];
	}

	T* data(const int idx)
	{
		return (m_stateVector.data() + idx);
	}

	const T* data(const int idx) const
	{
		return (m_stateVector.data() + idx);
	}

	const T& getDensityQuick(int idx) const
	{
		applyCondition(idx);
		return m_stateVector[idx];
	}

	const T& getDensity(int idx) const
	{
		applyCondition(idx);
		return m_stateVector.at(idx);
	}

	// this has to be signed it could be negative which we then wrap
	T& getDensity(int idx)
	{
		applyCondition(idx);
		return m_stateVector.at(idx);
	}

	void set(size_type idx, const T val)
		{ m_stateVector.at(idx) = val; }

	// wrap the boundary apply condition, called from shift op
	bool applyCondition(int& idx) const
	{
		return b->applyCondition(idx, domainSize);
	}

	size_type size() const { return m_stateVector.size(); }

};

// an attempt at a general shift operator!
template<int N, typename Dst>
struct ShiftOperator
{
	ShiftOperator(Dst& _dst) : dst(_dst) {}

	void operator() (int x)
	{
		assert((unsigned)x<dst.size() && x>=0);
		//ASSERT ((unsigned)x<dst.size() && x>=0, "index="<< x << " invalid density vector index!");
		auto nidx = x + N;

		//std::cout << " nidx="<<nidx << " N=" << N;

		dst.applyCondition(nidx);
		//std::cout << " nidx_post="<< nidx << std::endl;

		assert((unsigned)nidx<dst.size() && nidx>=0);
		//ASSERT_1((unsigned)nidx<dst.size() && nidx>=0);

		/*
		// DEBUG
		long before1 = dst[x];
		long before2 = dst[nidx];

		//std::cout << " before[" << x << "]=" << dst[x];
		//std::cout << " before[" << nidx << "]=" << dst[nidx];

		*/

		dst[x]-= (dst[x]>0) ? 1 : 0;
		dst[nidx]+=1;

		/*
		ASSERT((before1>0) ? before1-1==dst[x] : before1==0, "");
		ASSERT(before2+1==dst[nidx], "");

		//std::cout << " after[" << x << "]=" << dst[x];
		//std::cout << " after[" << nidx << "]=" << dst[nidx] << std::endl;

		dst.checkTotal();
		*/
	}

private:
	Dst& dst;
};

template<typename T>
class stateVector : public stateVector_impl<T>
{
	using state_type = stateVector_impl<T>;
	using size_type = typename stateVector_impl<T>::size_type;
	using stateVector_impl<T>::m_stateVector;

public:

	stateVector()
		: RightShift(*this), LeftShift(*this)
	{}

	stateVector(std::shared_ptr<Parameters> p)
		: stateVector_impl<T>(p), RightShift(*this), LeftShift(*this)
	{}

public:
	ShiftOperator<1, state_type> RightShift;
	ShiftOperator<-1, state_type> LeftShift;

};

#endif

