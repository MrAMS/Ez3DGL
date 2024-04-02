cmake_minimum_required(VERSION 3.0.0)
project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

include_directories(.)

include_directories(../../)

# glfw3
find_package(glfw3 REQUIRED)
include_directories(${glfw3_INCLUDE_DIR})

# glad
include_directories(./3rdparty/glad/include/)
add_subdirectory(./3rdparty/glad)

# glm
include_directories(./3rdparty/glm-0.9.9.8/)
add_subdirectory(./3rdparty/glm-0.9.9.8/)

# stb_image
include_directories(./3rdparty/stb_image/)

# imgui
include_directories(./3rdparty/imgui/imgui-1.89.4/)
include_directories(./3rdparty/imgui/imgui-1.89.4/backends/)
add_subdirectory(./3rdparty/imgui/)

# assimp
include_directories(./3rdparty/assimp/)
set(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory(./3rdparty/assimp/)

# Sanitizers
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(${PROJECT_NAME} main.cpp
								../../core/vertices_layer.cpp
								../../window/window.cpp
								../../utils/preset.cpp
								)

target_link_libraries(${PROJECT_NAME} 
						# 3rd party
						glfw glad glm imgui assimp)

target_include_directories(${PROJECT_NAME} PUBLIC
				"${PROJECT_BINARY_DIR}"
				)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)