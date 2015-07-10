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

using namespace boundary;

class RandomWalk;

class Simulator
{

    using data_type = std::vector<unsigned int>;
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
	void registerListener(const std::function<void (std::vector<unsigned int>)>& l);

    void print();
    void run();
    std::vector<unsigned int>& getPath() const;
    void notify(EventType&& e);

    void update(std::shared_ptr<Parameters> param_)
    {
        param = param_;
        init();
    }

    static EventRegistry eventRegistry;

private:
    // does this really have to be here??
    EventListener<data_type> listener;

    std::shared_ptr<Parameters> param;
    std::unique_ptr<RandomWalk> TheRandomWalk;
};


