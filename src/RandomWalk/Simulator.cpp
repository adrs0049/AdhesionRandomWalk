
#include "Simulator.h"
#include "RandomWalk.h"
#include <Loop.h>
#include <algorithm>
#include <iostream>
#include <memory>

#include "EventRegistry.h"
#include "Event.h"
#include "EventListener.h"
#include "Terminate.h"

#include "debug.h"

template<typename T>
const std::string stateVector_impl<T>::delta("Delta");
template<typename T>
const std::string stateVector_impl<T>::uniform("Uniform");

const std::string BoundaryFactory::no_flux("NoFlux");
const std::string BoundaryFactory::periodic("Periodic");

EventRegistry Simulator::eventRegistry;

Simulator::Simulator()
: param(nullptr), TheRandomWalk(nullptr)
{
    Error::TerminalCatcher::init();
}

Simulator::Simulator(std::shared_ptr<Parameters> p)
: param(p), TheRandomWalk(nullptr)
{
    Error::TerminalCatcher::init();
    ASSERT(p!=nullptr, "parameters is null");
	init();
    //std::cout << "DL(sim)=" << p.DomainSizeL << std::endl;
}

Simulator::~Simulator()
{}

void Simulator::init()
{
    //std::cout << "Simulator init" << std::endl;
    //std::cout << "Parameters are: ";
    //param->print_info();
    TheRandomWalk = std::make_unique<RandomWalk>(param);
    // setup event
    //auto& event = TheRandomWalk->getEvent();
    TheRandomWalk->setSimulator(this);
}

void Simulator::registerListener(PyCallback_Fcn& l)
{
	//log<LOG::DEBUG>("Registering Listener");
    std::cout << "Registering listener" << std::endl;
    //auto& event = TheRandomWalk->getEvent();
    eventRegistry.register_handler(EventType::new_data,
            std::make_shared<event_type>());

    auto event = std::static_pointer_cast<event_type>(eventRegistry.get_handler(EventType::new_data));
    listener = event->createListener(l);
}

void Simulator::print()
{
    auto path = TheRandomWalk->getCells();
    path->print();
}

void Simulator::run()
{
    try {
        TheRandomWalk->GeneratePath();
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

std::vector<unsigned int>& Simulator::getPath() const
{
    return TheRandomWalk->getPath();
}

void Simulator::notify(EventType&& e)
{
	if (eventRegistry.getNumberOfEvents()>0)
	{
		std::cout << "notify simulator" << std::endl;
		auto event = std::static_pointer_cast<event_type>(eventRegistry.get_handler(std::move(e)));
		event->notifyListeners({getPath(), param->getSteps(), param->getCurrentTime()});
	}
}

