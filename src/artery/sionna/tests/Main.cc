#include <artery/sionna/bridge/Helpers.h>
#include <backward.hpp>
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    backward::SignalHandling signal_handling;
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(catch_exceptions) = false;
    artery::sionna::ScopedInterpreter guard(SIONNA_VENV_HINT);
    return RUN_ALL_TESTS();
}
