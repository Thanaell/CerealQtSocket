cmake_minimum_required(VERSION 3.5)

project(testClientServer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)
find_package(OpenCV)
find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5Network)

include_directories(${cereal_DIR}/include)

add_executable(ProtoCereal main.cpp server.cpp server.h dataStructs.h)
add_executable(Client mainClient.cpp client.cpp client.h dataStructs.h)
target_link_libraries(ProtoCereal ${OpenCV_LIBS} Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui)
target_link_libraries(Client ${OpenCV_LIBS} Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui)
