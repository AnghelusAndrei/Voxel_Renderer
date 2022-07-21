cmake_minimum_required(VERSION 3.7)
project(Voxel_Renderer VERSION 1.0) 

set(CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


#set(CMAKE_BUILD_TYPE Debug)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()


add_executable(${PROJECT_NAME} src/main.cpp)

if(WIN32)
    find_package(SDL2TTF)
else()
    find_package(SDL_ttf REQUIRED)
endif()

find_package(SDL2 REQUIRED)
find_package(OpenCL REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL_TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL_TTF_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)


target_include_directories(${PROJECT_NAME} PUBLIC lib/PerlinNoise)

file(GLOB_RECURSE shaders "*.cl" "*.hcl")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shd)
