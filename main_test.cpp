#include <gtest/gtest.h>
#include "state_a.hpp"
#include "state_b.hpp"
#include "event.hpp"


namespace Nikos {

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

    int main(int argc, char **argv) {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
}