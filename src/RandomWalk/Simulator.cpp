
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
#include "exceptions.h"
#include "vector.h"

#include <spdlog.h>

EventRegistry Simulator::eventRegistry;

Simulator::Simulator()
: param(nullptr), TheRandomWalk(nullptr), id(0)
	//logger(spdlog::stdout_logger_st("simulator_log"))
{
    Error::TerminalCatcher::init();
}

Simulator::Simulator(std::shared_ptr<Parameters> p)
: param(p), TheRandomWalk(nullptr), id(0)
	//logger(spdlog::stdout_logger_st("simulator_log"))
{
    Error::TerminalCatcher::init();
	ASSERT(p!=nullptr, "parameters is null");
	init();
    //std::cout << "DL(sim)=" << p.DomainSizeL << std::endl;
}

Simulator::~Simulator()
{}
/*
void Simulator::set_logger_level(void)
{
	switch (debug_level)
	{
		case DebugLevel::DEBUG:
			spdlog::set_level(spdlog::level::debug);
			break;
		case DebugLevel::NORMAL:
			spdlog::set_level(spdlog::level::notice);
			break;
		case DebugLevel::VERBOSE:
			spdlog::set_level(spdlog::level::info);
			break;
		case DebugLevel::QUIET:
			spdlog::set_level(spdlog::level::off);
			break;
		default:
			throw NotImplementedException {""};
			break;
	};
}
*/
void Simulator::init()
{
	//set_logger_level();
	//logger->info("Simulator init");
	//logger->info("The parameters are...");
    //param->print_info();
    TheRandomWalk = std::make_unique<RandomWalk>(param);
    TheRandomWalk->setSimulator(this);
}

void Simulator::registerListener(PyCallback_Fcn& l)
{
	//logger->debug("Registering Listener");
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
		//logger->error(e.what());
    }
}

AligndVector<unsigned int> Simulator::getPath() const
{
    return TheRandomWalk->getPath();
}

void Simulator::notify(EventType&& e)
{
	if (eventRegistry.getNumberOfEvents()>0)
	{
		auto event = std::static_pointer_cast<event_type>
			(eventRegistry.get_handler(std::move(e)));
		event->notifyListeners({getPath(), param->getSteps(),
				param->getCurrentTime()});
	}
	else
	{
		//logger->warn("No Listener is defined!");
	}
}

