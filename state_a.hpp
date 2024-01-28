#pragma once

#include "state.hpp"

namespace Nikos {
    class EventAtoB;

    class StateA : public ConcState<StateA> {
        public:
        State * processConcreteEvent(EventAtoB *e);
        State * processConcreteEvent(Event *e);
    };
}