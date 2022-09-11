cmake_minimum_required(VERSION 3.23)
project(YandexDisk)

set(CMAKE_CXX_STANDARD 20)

file(GLOB headers src/*.h)
file(GLOB sources src/*.cpp)

add_executable(${PROJECT_NAME} ${sources} ${headers})
