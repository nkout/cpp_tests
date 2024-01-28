#pragma once

#include <string>

namespace Nikos {
    class Event;

    class State {
        public:
        virtual State * processEvent(Event *e) = 0;
        virtual const std::string toStr() = 0;
        virtual ~State() = default;
    };

    template <typename T>
    class ConcState : public State 
    {
        public:
        static State * getInst();
        State * processEvent(Event *e) override;
        const std::string toStr() override;
    };
}