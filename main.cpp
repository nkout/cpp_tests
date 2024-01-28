#include <iostream>
#include <memory>
#include "state_a.hpp"
#include "event.hpp"

using namespace Nikos;
using namespace std;

int main(void)
{
    EventAtoB e1;
    EventBtoA e2;

    Event *pEventAtoB = &e1;
    Event *pEventBtoA = &e2;

    State *s = StateA::getInst();
    cout << s->toStr() << endl << endl;

    s = s->processEvent(pEventBtoA);
    cout << s->toStr() << endl << endl;

    s = s->processEvent(pEventAtoB);
    cout << s->toStr() << endl << endl;

    s = s->processEvent(pEventAtoB);
    cout << s->toStr() << endl << endl;

    s = s->processEvent(pEventBtoA);
    cout << s->toStr() << endl;
}
