#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "state_a.hpp"
#include "state_b.hpp"
#include "event.hpp"

namespace Nikos {

    class MockState : public State{
    public:
        MOCK_METHOD( (State *), processEvent, (Event *e), (override));
        MOCK_METHOD( (const std::string), toStr, (), (override));

    };

// The fixture for testing class Foo.
    class StateTest : public testing::Test {
    protected:
        // You can remove any or all of the following functions if their bodies would
        // be empty.

        StateTest() : initState(StateA::getInst()), currState(nullptr) {}
        ~StateTest() override {}

        void SetUp() override {
            currState = initState;
        }

        void TearDown() override {
        }

        State *initState;
        State *currState;
    };

    TEST_F(StateTest, MethodHandleEvent) {
        EXPECT_EQ(currState->toStr(), StateA::getInst()->toStr());

        EventAtoB eSpecific;
        Event *e = &eSpecific;
        currState = currState->processEvent(e);

        EXPECT_EQ(currState->toStr(), StateB::getInst()->toStr());
        EXPECT_NE(dynamic_cast<StateB*>(currState), nullptr);
        EXPECT_NE(dynamic_cast<State*>(currState), nullptr);
        EXPECT_EQ(dynamic_cast<StateA*>(currState), nullptr);
    }

    TEST_F(StateTest, MethodHandleDefaultEvent) {
        EXPECT_EQ(currState->toStr(), StateA::getInst()->toStr());

        EventBtoA eSpecific;
        Event *e = &eSpecific;
        currState = currState->processEvent(e);

        EXPECT_EQ(currState->toStr(), StateA::getInst()->toStr());
    }

    TEST_F(StateTest, MethodHandleMock) {
        MockState state;

        EXPECT_CALL(state, toStr())
            .Times(1)
            .WillOnce([](void) {return StateA::getInst()->toStr();});

        EXPECT_EQ(state.toStr(), StateA::getInst()->toStr());
    }
}