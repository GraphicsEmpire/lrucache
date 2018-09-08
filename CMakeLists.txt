cmake_minimum_required(3.2)

project(lrucache)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/lrucache.cpp
                               ${CMAKE_SOURCE_DIR}/src/main.cpp)