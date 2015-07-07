#include <functional>
#include <string>
#include <map>


typedef std::function<void (std::string)> EventListener;

enum Event { THING };

inline std::map<Event, EventListener>& table()
{
    static std::map<Event, EventListener> map;
    return map;
}

inline const EventListener& register_handler(const Event& e, const EventListener& l)
{
    return table()[e] = l;
}

inline void test(const Event& e)
{
    table()[e]("Testing");
}



