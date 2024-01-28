#pragma once

#include "state_a.hpp"
#include "state_b.hpp"

namespace Nikos {
 

    class Event {
        public:
        virtual ~Event() = default;
        template <typename U>
        State * stateChng(U *s) {return s->processConcreteEvent(this);}

        //add all states here and class below - only specific states, not (State *) param
        virtual State * triggerStateChange(StateA *s) {return stateChng(s);}
        virtual State * triggerStateChange(StateB *s) {return stateChng(s);}
    };


    template <typename T>
    class ConcEv : public Event {
        public:
        template <typename U>
        State * stateChng(U *s) {return s->processConcreteEvent(static_cast<T*>(this));}

        //add all states here and class above - only specific states, not (State *) param
        State * triggerStateChange(StateA *s) override {return stateChng(s);}
        State * triggerStateChange(StateB *s) override {return stateChng(s);}
    };


    //add all concrete events here
    class EventBtoA : public ConcEv<EventBtoA> {};
    class EventAtoB : public ConcEv<EventAtoB> {};
}