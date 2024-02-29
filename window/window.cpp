#include <cstdio>
#include "core/vertices_layer.hpp"
#include <vector>
#include <filesystem>
#include <string>
#include "utils/debug.hpp"

#include "window.hpp"

using namespace Ez3DGL;


glfw_win_t::glfw_win_t(int width_, int height_, const char* title_):width(width_), height(height_), title(title_){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int glfw_win_t::show(int (*setup)(), int (*loop)(), int (*exit)()){
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(window==NULL){
        printf("Failed to create Window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    int ret = setup();
    if(ret!=0) return ret;

    // Render loop
    while(!glfwWindowShouldClose(window)){
        float current_frame = glfwGetTime();
        frame_time_delta = current_frame - frame_time_last;
        frame_time_last = current_frame;

        glfwPollEvents();
        
        ret = loop();
        if(ret!=0) return ret;

        
        glfwSwapBuffers(window);
    }

    ret = exit();
    if(ret!=0) return ret;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


static glfw_win_t* win;
static camera_t* camera;

static bool ign_keyboard = false, ign_mouse = false;

static void framebuffer_size_callback(GLFWwindow* win, int w, int h){
    glViewport(0, 0, w, h);
    extern void window_framebuffer_size_callback(int w, int h);
    window_framebuffer_size_callback(w, h);
}

static void _key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(ign_keyboard) return;
    extern void window_key_callback(int key, int scancode, int action, int mods);
    window_key_callback(key, scancode, action, mods);
}

static void _mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(ign_mouse) return;
    extern void window_mouse_callback(double xpos, double ypos);
    window_mouse_callback(xpos, ypos);
}

static void _scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if(ign_mouse) return;
    extern void window_scroll_callback(double xoffset, double yoffset);
    window_scroll_callback(xoffset, yoffset);
}
int window_setup(){
    glfwMakeContextCurrent(win->window);
    glfwSetFramebufferSizeCallback(win->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(win->window, _mouse_callback);
    glfwSetScrollCallback(win->window, _scroll_callback);
    glfwSetKeyCallback(win->window, _key_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(win->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to init GLAD\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    extern void user_setup();
    user_setup();

    return 0;
}

int window_loop(){
    extern void user_imgui();
    user_imgui();

    auto io = ImGui::GetIO();
    if(io.WantCaptureMouse)
        ign_mouse = true;
    else
        ign_mouse = false;
    
    if(io.WantCaptureKeyboard)
        ign_keyboard = true;
    else
        ign_keyboard = false;
    
    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    extern void user_loop();
    user_loop();

//    printf("camera pos: %f %f %f\n", camera->position.x, camera->position.y, camera->position.z);
//    printf("camera fov: %f\n", camera->fov);


    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return 0;
}


int window_exit(){

    extern void user_exit();
    user_exit();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

int window_launch(const char* title, int win_width, int win_height){
    win = new glfw_win_t(win_width, win_height, title);
    return win->show(window_setup, window_loop, window_exit);
}

static void implement_tip(){
    // log_with_info("Maybe implement the function yourself");
}

__attribute__((weak)) void user_setup(){
    implement_tip();
}

__attribute__((weak)) void user_imgui(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
}

__attribute__((weak)) void user_loop(){
    implement_tip();
}

__attribute__((weak)) void user_exit(){
    implement_tip();
}
__attribute__((weak)) void window_key_callback(int key, int scancode, int action, int mods){
    implement_tip();
}
__attribute__((weak)) void window_mouse_callback(double xpos, double ypos){
    implement_tip();
}
__attribute__((weak)) void window_scroll_callback(double xoffset, double yoffset){
    implement_tip();
}

__attribute__((weak)) void window_framebuffer_size_callback(int w, int h){
    implement_tip();
}