cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)

include(FetchContent)

Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.7z # downloading a zip release speeds up the download
        USES_TERMINAL_DOWNLOAD TRUE
        GIT_PROGRESS TRUE
        DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)


project(MyProject)

Set(MyProject_SRC
        state_a.cpp
        state_b.cpp
)

Set(MyProject_Test_SRC
        state_test.cpp
)

Set(MyProject_BoostLibs
        Boost::filesystem
        Boost::program_options
)

add_executable(main_states
        main_states.cpp
        ${MyProject_SRC}
)
target_link_libraries(main_states PRIVATE
        ${MyProject_BoostLibs}
)

add_executable(main_threads
        main_threads.cpp
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()

add_executable(main_test
        ${MyProject_Test_SRC}
        ${MyProject_SRC}
)
target_link_libraries(main_test
        GTest::gtest_main
        gmock_main
        ${MyProject_BoostLibs}
)
add_executable(main_ring_buff main_ring_buffer.cpp
        mutex_ring_buffer.cpp
        atomic_ring_buffer.cpp)
