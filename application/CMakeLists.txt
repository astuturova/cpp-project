cmake_minimum_required(VERSION 3.26)

project(test_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/Users/denisborisov/Qt/6.7.0/macos/lib/cmake")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

set(BOOST_ROOT /usr/local/Cellar/boost/1.85.0)
set(BOOST_INCLUDEDIR /usr/local/Cellar/boost/1.85.0/include)
set(BOOST_LIBRARY_DIR /usr/local/Cellar/boost/1.85.0/lib)

find_package(Boost 1.85.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif ()

set(OPENSSL_ROOT_DIR  /usr/local/Cellar/openssl@3/3.3.1)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(test_1 main.cpp
        models/user.h
        models/user.cpp
        models/password.h
        models/password.cpp)
target_link_libraries(test_1
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        ${Boost_LIBRARIES}
        ${OPENSSL_LIBRARIES}
)