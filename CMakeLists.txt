cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)
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

Set(MyProject_BoostLibs
        Boost::filesystem
        Boost::program_options
)

add_executable(main
        main.cpp
        ${MyProject_SRC}
)
target_link_libraries(main PRIVATE
        ${MyProject_BoostLibs}
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()

add_executable(main_test
        main_test.cpp
        ${MyProject_SRC}
)
target_link_libraries(main_test
        GTest::gtest_main
        ${MyProject_BoostLibs}
)