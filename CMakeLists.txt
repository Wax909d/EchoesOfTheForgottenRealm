cmake_minimum_required(VERSION 3.16)
project(EchoesOfTheForgottenRealm CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML 2.6. Try the normal search first so this builds on any machine;
# only fall back to known Homebrew locations if that fails (some Macs have
# both SFML 2.6 and 3.0 installed and need the 2.6 path hinted explicitly).
find_package(SFML 2.6 COMPONENTS graphics window system audio QUIET)
if(NOT SFML_FOUND)
    foreach(hint
        "/opt/homebrew/opt/sfml@2/lib/cmake/SFML"
        "/opt/homebrew/Cellar/sfml@2/2.6.2_1/lib/cmake/SFML"
        "/usr/local/opt/sfml@2/lib/cmake/SFML")
        if(EXISTS "${hint}")
            set(SFML_DIR "${hint}")
        endif()
    endforeach()
    find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
endif()

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Config.cpp
    src/Animation.cpp
    src/TileMap.cpp
    src/TextureManager.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Slime.cpp
    src/Archer.cpp
    src/Skeleton.cpp
    src/Projectile.cpp
    src/Pickups.cpp
    src/AudioManager.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Copy assets next to the binary after EVERY build, so swapping an asset and
# pressing Run in CLion picks it up without re-running CMake.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
