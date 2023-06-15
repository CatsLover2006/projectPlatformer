cmake_minimum_required(VERSION 3.0)
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "-std=c++20 ${CMAKE_CXX_FLAGS}")
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(MACOSX_BUNDLE_BUNDLE_NAME "Project Platformer")
set(MACOSX_BUNDLE_BUNDLE_VERSION 0.0.0)
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "Indev v0")
project(projectPlatformer)

if(MINGW OR MSYS)
set(SDL2_PATH "./lib/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "./lib/x86_64-w64-mingw32")
set(SDL2_TTF_PATH "./lib/x86_64-w64-mingw32")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

set(sourceFiles ${sourceFiles} 
    hailLib/sdlWrapper.cpp
    hailLib/basemath.cpp
    main.cpp
    collider.cpp
    object.cpp
    playerInteractableObjects.cpp
    player.cpp
    crib.cpp
    loadLevel.cpp
    orc.cpp
    door.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})

set(ARCH_LIBS "")
set(POST_ARCH_LIBS "")

# Executable
if (APPLE)
add_executable(projectPlatformer MACOSX_BUNDLE ${sourceFiles})
add_custom_command(
      TARGET projectPlatformer
      POST_BUILD
      COMMAND plutil -replace NSHighResolutionCapable -bool true $<TARGET_FILE_DIR:${PROJECT_NAME}>/../Info.plist
      )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/assets"
)
elseif(MSYS)
add_executable(projectPlatformer WIN32 ${sourceFiles})
endif()

target_link_libraries(projectPlatformer ${ARCH_LIBS}
				     ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             ${SDL2_TTF_LIBRARIES}
				     ${POST_ARCH_LIBS})