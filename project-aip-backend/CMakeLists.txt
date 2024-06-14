cmake_minimum_required(VERSION 3.26)
project(project_aip_backend)

add_executable(project_aip_backend main.cpp include/json.hpp
        api/database.cpp api/database.h models/password.cpp models/password.h include/sqlite3.c models/user.cpp models/user.h
        api/encryptor.h
        api/encryptor.cpp
        tests/crypto_tests.cpp
        include/doctest.h)

set(CMAKE_CXX_STANDARD 11)

set(BOOST_ROOT /usr/local/Cellar/boost/1.85.0)
set(BOOST_INCLUDEDIR /usr/local/Cellar/boost/1.85.0/include)
set(BOOST_LIBRARY_DIR /usr/local/Cellar/boost/1.85.0/lib)

find_package(Boost 1.85.0 COMPONENTS system filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif ()


find_package(PkgConfig REQUIRED)




