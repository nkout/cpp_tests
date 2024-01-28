#include <iostream>
#include "event.hpp"
#include "state_a.hpp"
#include "state_b.hpp"
#include "state_impl.hpp"

namespace Nikos {
    State * StateA::processConcreteEvent(Event *e)
    {
        //std::cout << "A:unknown Event" << std::endl;
        return this;
    }

    State * StateA::processConcreteEvent(EventAtoB *e)
    {
        //Action
        //
        //std::cout << "A:EventAtoB" << std::endl;


        //State change
        return StateB::getInst();
    }
}