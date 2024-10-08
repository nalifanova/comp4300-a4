cmake_minimum_required(VERSION 3.28)
# Name is Game17 'coz of the number of the lesson
project(Game17 LANGUAGES CXX)

# require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ensure -std=c++XX instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-reorder")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-reorder")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
option(SFML_BUILD_AUDIO "Build audio" ON)
FetchContent_MakeAvailable(SFML)

## Adding imggui and imgui-sfml
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG     v1.91.0)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG        2.6.x
)

# Dear ImGui
FetchContent_MakeAvailable(imgui)

# ImGui-SFML
set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)

FetchContent_MakeAvailable(imgui-sfml)

include_directories(include)
include_directories(assets)

add_executable(${PROJECT_NAME}
    main.cpp
    include/Action.hpp
    include/Animation.hpp
    include/Assets.hpp
    include/Components.hpp
    include/Entity.hpp
    include/EntityManager.hpp
    include/GameEngine.hpp
    include/Physics.hpp
    include/Scene.hpp
    include/SceneMenu.hpp
    include/SceneZelda.hpp
    include/Vec2.hpp
    src/Action.cpp
    src/Animation.cpp
    src/Assets.cpp
    src/Entity.cpp
    src/EntityManager.cpp
    src/GameEngine.cpp
    src/Physics.cpp
    src/Scene.cpp
    src/SceneMenu.cpp
    src/SceneZelda.cpp
    src/Vec2.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC ImGui-SFML::ImGui-SFML)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-audio)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()

file(GLOB fonts ${CMAKE_SOURCE_DIR}/assets/fonts/*.ttf)
file(GLOB images ${CMAKE_SOURCE_DIR}/assets/images/*.png)
file(GLOB sounds ${CMAKE_SOURCE_DIR}/assets/sounds/*.*)
file(GLOB texts ${CMAKE_SOURCE_DIR}/config/*.txt)

file(COPY ${fonts} DESTINATION ${CMAKE_BINARY_DIR}/bin/assets/fonts)
file(COPY ${images} DESTINATION ${CMAKE_BINARY_DIR}/bin/assets/images)
file(COPY ${sounds} DESTINATION ${CMAKE_BINARY_DIR}/bin/assets/sounds)
file(COPY ${texts} DESTINATION ${CMAKE_BINARY_DIR}/bin/config)
