#pragma once

#include "state.hpp"

namespace Nikos {
    class EventBtoA;
    class StateB : public ConcState<StateB> {
        public:
        State * processConcreteEvent(EventBtoA *e);
        State * processConcreteEvent(Event *e);
    };
}