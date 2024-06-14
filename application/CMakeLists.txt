cmake_minimum_required(VERSION 3.26)

project(test_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

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