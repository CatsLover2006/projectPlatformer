cmake_minimum_required(VERSION 3.0)
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(MACOSX_BUNDLE_BUNDLE_NAME "Project Platformer")
set(MACOSX_BUNDLE_BUNDLE_VERSION 0.0.0)
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "Indev v0")
project(projectPlatformer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

set(sourceFiles ${sourceFiles} 
    hailLib/sdlWrapper.cpp
    hailLib/basemath.cpp
    main.cpp
    loadLevel.cpp
    collider.cpp
    object.cpp
    player.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Create executable
add_executable(projectPlatformer MACOSX_BUNDLE ${sourceFiles})

# Link libs
target_link_libraries(projectPlatformer ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

# Copy resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/assets"
)