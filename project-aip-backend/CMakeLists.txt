cmake_minimum_required(VERSION 3.26)
project(project_aip_backend)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.85.0)
set(BOOST_INCLUDEDIR /opt/homebrew/Cellar/boost/1.85.0/include)
set(BOOST_LIBRARY_DIR /opt/homebrew/Cellar/boost/1.85.0/lib)

find_package(Boost 1.85.0 COMPONENTS system filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif()

add_executable(project_aip_backend main.cpp json.hpp)
