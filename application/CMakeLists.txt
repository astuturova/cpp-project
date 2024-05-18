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
        REQUIRED)

add_executable(test_1 main.cpp)
target_link_libraries(test_1
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

