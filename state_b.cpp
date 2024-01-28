#include <iostream>
#include "event.hpp"
#include "state_b.hpp"
#include "state_a.hpp"
#include "state_impl.hpp"

namespace Nikos {
    State * StateB::processConcreteEvent(Event *e)
    {
        //std::cout << "B: unknown Event" << std::endl;
        return this;
    }

    State * StateB::processConcreteEvent(EventBtoA *e)
    {
        //Action
        //
        //std::cout << "B: EventBtoA" << std::endl;

        //State change
        return StateA::getInst();
    }
}