// simulator class
//
//

#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <Loop.h>

#include <ParkMiller.h>

#include <PeriodicBoundary.h>
#include <Boundary.h>
#include <BoundaryFactory.h>

#include "RandomWalk.h"
#include "Parameters.h"
#include <memory>
#include <functional>
#include "make_unique.h"

#include "EventRegistry.h"
#include "Event.h"
#include "EventListener.h"
#include "SimulationData.h"
#include "vector.h"

#include <spdlog.h>

enum class DebugLevel : int
{
	DEBUG,
	VERBOSE,
	NORMAL,
	QUIET,
};

using namespace boundary;

class RandomWalk;

typedef const std::function<void (const SimulationData)> PyCallback_Fcn;

class Simulator
{
	typedef SimulationData data_type;
    //using data_type = Data;
    using event_type = Event<data_type>;

public:
    Simulator();
    Simulator(std::shared_ptr<Parameters> p );

    //~Simulator() = default;
    ~Simulator();

    Simulator(const Simulator& other) = delete;
    Simulator(Simulator&& other) = delete;
    Simulator& operator=(const Simulator& rhs) = delete;
    Simulator& operator=(Simulator&& rhs) = delete;

    void init();
	void registerListener(PyCallback_Fcn& l);

    void print();
    void run();
	AligndVector<unsigned int> getPath() const;
    //std::vector<unsigned int>& getPath() const;
    void notify(EventType&& e);

    void update(std::shared_ptr<Parameters> param_)
    {
        param = param_;
        init();
    }

private:
	void set_logger_level(void);

    // does this really have to be here??
    EventListener<data_type> listener;
	// event registry private, should this be something else?
	// Swig tries to include this if not private
	static EventRegistry eventRegistry;

    std::shared_ptr<Parameters> param;
    std::unique_ptr<RandomWalk> TheRandomWalk;

	std::shared_ptr<spdlog::logger> logger;
	DebugLevel debug_level = DebugLevel::DEBUG;

	const unsigned int id;
	const std::string parent_name;
};
