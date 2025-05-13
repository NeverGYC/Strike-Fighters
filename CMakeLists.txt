cmake_minimum_required(VERSION 3.18)

# Display name of project (for documentation, etc.)
project("Strike Fighters")

# Use a valid target name without spaces
set(PROJ_NAME "Strike_Fighters")

# Specify project files: header files and source files
set(HDRS
    file_utils.h
    game.h
    game_object.h
    player_game_object.h
    shader.h
    geometry.h
    sprite.h
    timer.h
    audio_system.h
    collectible_game_object.h
    enemy_game_object.h
    projectile_game_object.h
    ray_circle_collision.h
    particles.h
    fast_enemy_game_object.h
    heavy_enemy_game_object.h
    homing_missile.h
    asteroid_game_object.h
    text_game_object.h
    game_timer.h
    enemy_counter.h 
    health_bar.h
    boss_game_object.h
    space_dust.h
    missile_flame.h
    mini_map.h
    minimap_sprite.h
)
 
set(SRCS
    file_utils.cpp
    game.cpp
    game_object.cpp
    main.cpp
    player_game_object.cpp
    shader.cpp
    sprite.cpp
    sprite_vertex_shader.glsl
    sprite_fragment_shader.glsl
    text_fragment_shader.glsl
    particle_fragment_shader.glsl
    particle_vertex_shader.glsl
    dust_fragment_shader.glsl
    minimap_vertex_shader.glsl
    minimap_fragment_shader.glsl
    timer.cpp
    audio_system.cpp
    collectible_game_object.cpp
    enemy_game_object.cpp
    projectile_game_object.cpp
    fast_enemy_game_object.cpp
    heavy_enemy_game_object.cpp
    homing_missile.cpp
    asteroid_game_object.cpp
    text_game_object.cpp
    game_timer.cpp 
    enemy_counter.cpp 
    health_bar.cpp
    boss_game_object.cpp
    space_dust.cpp
    missile_flame.cpp
    mini_map.cpp
)


# Add path name to configuration file
configure_file(path_config.h.in path_config.h)

# Add executable based on the source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})

# Directories to include for header files, so that the compiler can find
# path_config.h
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Require OpenGL library
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

# Other libraries needed
set(LIBRARY_PATH $ENV{COMP2501_LIBRARY_PATH} CACHE PATH "Folder with GLEW, GLFW, GLM, and SOIL libraries")
include_directories(${LIBRARY_PATH}/include)

# Find required libraries
if(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
else(WIN32)
    find_library(GLEW_LIBRARY GLEW HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
endif(WIN32)

# Add OpenAL and ALUT
if(NOT WIN32)
    find_package(OpenAL REQUIRED)
    find_package(ALUT REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIR} ${ALUT_INCLUDE_DIR})
    target_link_libraries(${PROJ_NAME} ${OPENAL_LIBRARY} ${ALUT_LIBRARY})
else(NOT WIN32)
    find_library(OPENAL_LIBRARY OpenAL32 HINTS ${LIBRARY_PATH}/lib)
    find_library(ALUT_LIBRARY alut HINTS ${LIBRARY_PATH}/lib)
    target_link_libraries(${PROJ_NAME} ${OPENAL_LIBRARY} ${ALUT_LIBRARY})
endif(NOT WIN32)

# Link libraries
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)

    # Set the default project in VS
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJ_NAME})
endif(WIN32)