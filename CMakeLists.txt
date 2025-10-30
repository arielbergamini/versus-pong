cmake_minimum_required(VERSION 3.16)
project(versuspong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Ariel Bergamini/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(CMAKE_PREFIX_PATH "C:/Users/Ariel Bergamini/vcpkg/installed/x64-mingw-dynamic")

find_package(SDL2 CONFIG REQUIRED)

#build target
add_executable(pong
    src/main.cpp
    src/game.cpp
)

#include & link sdl2
target_include_directories(pong PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(pong PRIVATE SDL2::SDL2main SDL2::SDL2)

#copy assets & required dlls after each build
add_custom_command(TARGET pong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/ucrt64/bin/libgcc_s_seh-1.dll"
        "C:/msys64/ucrt64/bin/libstdc++-6.dll"
        "C:/msys64/ucrt64/bin/libwinpthread-1.dll"
        "C:/Users/Ariel Bergamini/vcpkg/installed/x64-mingw-dynamic/bin/SDL2.dll"
        $<TARGET_FILE_DIR:pong>
    COMMENT "cp mingw, assets, & required dlls"
)


# ---- Run target ----
add_custom_target(run
    COMMAND pong
    DEPENDS pong
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running .exe from project root..."
)
