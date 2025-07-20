cmake_minimum_required(VERSION 3.5)
project(VXGI-Renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(GLFW_ROOT_DIR ${CMAKE_SOURCE_DIR}/vendor/glfw)
set(GLEW_ROOT_DIR ${CMAKE_SOURCE_DIR}/vendor/glew)
set(ASSIMP_ROOT_DIR ${CMAKE_SOURCE_DIR}/vendor/assimp)
set(GLM_ROOT_DIR ${CMAKE_SOURCE_DIR}/vendor/glm)
set(STB_ROOT_DIR ${CMAKE_SOURCE_DIR}/vendor/stb)

find_package(OpenGL REQUIRED)

add_executable(Renderer
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_SOURCE_DIR}/src/Shader.cpp
)

target_include_directories(Renderer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${GLFW_ROOT_DIR}/include
    ${GLEW_ROOT_DIR}/include
    ${ASSIMP_ROOT_DIR}/include
    ${GLM_ROOT_DIR}
    ${STB_ROOT_DIR}
)

target_link_directories(Renderer PRIVATE
    ${GLFW_ROOT_DIR}/lib-vc2022
    ${GLEW_ROOT_DIR}/lib
    ${ASSIMP_ROOT_DIR}/lib
)

target_compile_definitions(Renderer PRIVATE
    GLEW_STATIC
)

target_link_libraries(Renderer PRIVATE
    opengl32.lib
    glfw3.lib
    glew32s.lib
    assimp-vc143-mtd.lib
)

add_custom_command(TARGET Renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${ASSIMP_ROOT_DIR}/lib/assimp-vc143-mtd.dll"
        "$<TARGET_FILE_DIR:Renderer>"
)