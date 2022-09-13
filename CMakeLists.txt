cmake_minimum_required(VERSION 3.23)
project(YandexDisk)

set(CMAKE_CXX_STANDARD 20)

file(GLOB headers src/*.h)
file(GLOB sources src/*.cpp)

find_package(cpprestsdk REQUIRED)

add_executable(${PROJECT_NAME} src/IDiskFacade.h ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} PRIVATE cpprestsdk::cpprest -lpqxx -lpq)

