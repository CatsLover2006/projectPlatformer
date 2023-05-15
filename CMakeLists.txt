cmake_minimum_required(VERSION 3.0)
set(CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
project(projectPlatformer)

set(sourceFiles ${sourceFiles} 
    sdlWrapper.cpp
    main.cpp
    hailEngine/line.cpp
    hailEngine/base.cpp
    hailEngine/collider.cpp
    hailEngine/object.cpp
)

# SDL check
option(SDL_VENDORED "Use vendor SDL2" OFF)
if(SDL_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
endif()

# Create executable
add_executable(projectPlatformer MACOSX_BUNDLE ${sourceFiles})

# Link libraries
target_link_libraries(projectPlatformer PRIVATE SDL2::SDL2)