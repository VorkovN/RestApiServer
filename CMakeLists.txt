cmake_minimum_required(VERSION 3.16.3)
project(YandexDisk)

set(CMAKE_CXX_STANDARD 20)

file(GLOB headers src/*.h)
file(GLOB sources src/*.cpp)

find_package(cpprestsdk REQUIRED )


add_executable(${PROJECT_NAME} ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} PRIVATE cpprestsdk::cpprest -lpqxx -lpq)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin)