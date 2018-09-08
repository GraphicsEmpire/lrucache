cmake_minimum_required(VERSION 3.2)

project(lrucache)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/lrucache.cpp
                               ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} pthread)
