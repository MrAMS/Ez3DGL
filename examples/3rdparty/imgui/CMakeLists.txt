set(SOURCE_DIR ./imgui-1.89.4)
include_directories(${SOURCE_DIR})
include_directories(${SOURCE_DIR}/backends)

add_library( imgui  ${SOURCE_DIR}/imgui.cpp
                    ${SOURCE_DIR}/imgui_demo.cpp
                    ${SOURCE_DIR}/imgui_tables.cpp
                    ${SOURCE_DIR}/imgui_widgets.cpp
                    ${SOURCE_DIR}/imgui_draw.cpp
                    ${SOURCE_DIR}/backends/imgui_impl_glfw.cpp
                    ${SOURCE_DIR}/backends/imgui_impl_opengl3.cpp )