#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Ez3DGL {
/**
 * @brief 窗口对象,自动管理GLFWwindow,ImGui,OpenGL,
 暴露给用户setup(启动时调用),loop(渲染时调用),exit(退出时调用)接口, 以及键盘输入,鼠标等接口
 * 
 */
class glfw_win_t{
    public:
        int width, height;
        const char* title;
        // time between current frame and last frame
        float frame_time_delta = 0.0f;
        float frame_time_last = 0.0f;
        bool imgui_wantCaptureMouse;
        bool imgui_wantCaptureKeyboard;

        GLFWwindow* window;

        glfw_win_t(int width, int height, const char* title);

        int show(int (*setup)(), int (*loop)(), int (*exit)());
};
}

int window_setup();
int window_loop();
int window_exit();
int window_launch(const char* title, int win_width, int win_height);
void window_key_callback(int key, int scancode, int action, int mods);
void window_mouse_callback(double xpos, double ypos);
void window_scroll_callback(double xoffset, double yoffset);
void window_framebuffer_size_callback(int w, int h);

