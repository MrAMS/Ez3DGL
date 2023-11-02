#include <cstdio>
#include "core/vertices_layer.hpp"
#include <vector>
#include <filesystem>
#include <string>

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

bool ign_keyboard = false, ign_mouse = false;
bool look_around = true;

static void framebuffer_size_callback(GLFWwindow* win, int w, int h){
    glViewport(0, 0, w, h);
    camera->screen_w_div_h = (float)w*1.0f/(float)h;
}

static void processInput(GLFWwindow* window){
    if(ign_keyboard) return;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->input_pos(camera_t::UP, win->frame_time_delta);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->input_pos(camera_t::DOWN, win->frame_time_delta);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->input_pos(camera_t::RIGHT, win->frame_time_delta);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->input_pos(camera_t::LEFT, win->frame_time_delta);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(ign_keyboard) return;
    if(action == GLFW_PRESS){
        switch (key) {
        case GLFW_KEY_ESCAPE: look_around = !look_around;
        break;
        }
    }
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(ign_mouse || !look_around) return;
    camera->input_pitch_yaw(xpos, ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if(ign_mouse) return;
    camera->input_fov(yoffset);
}

int window_setup(){
    glfwMakeContextCurrent(win->window);
    glfwSetFramebufferSizeCallback(win->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(win->window, mouse_callback);
    glfwSetScrollCallback(win->window, scroll_callback);
    glfwSetKeyCallback(win->window, key_callback);

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

    processInput(win->window);
    
    
    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    camera->calc_view();
    camera->calc_projection();

    extern void user_loop(camera_t* camera);
    user_loop(camera);

//    printf("camera pos: %f %f %f\n", camera->position.x, camera->position.y, camera->position.z);
//    printf("camera fov: %f\n", camera->fov);


    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return 0;
}


int window_exit(){
    // delete win;
    delete camera;

    extern void user_exit();
    user_exit();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

int window_launch(const char* title, glm::vec3 camera_position, int win_width, int win_height){
    win = new glfw_win_t(win_width, win_height, title);
    camera = new camera_t(win_width*1.0f/win_height, camera_position);
    return win->show(window_setup, window_loop, window_exit);
}

__attribute__((weak)) void user_setup(){
    // Please implement the function yourself
}

__attribute__((weak)) void user_imgui(){
    // Please implement the function yourself
}

__attribute__((weak)) void user_loop(camera_t* camera){
    // Please implement the function yourself
}

__attribute__((weak)) void user_exit(){
    // Please implement the function yourself
}
