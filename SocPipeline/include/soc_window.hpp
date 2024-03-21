#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//std
#include <string>

namespace soc{
    class SocWindow
    {
    public:
        SocWindow(int w,int h,std::string name);
        ~SocWindow();

        SocWindow(const SocWindow &) = delete;
        SocWindow &operation(const SocWindow &) = delete;

        bool shouldClose(){return glfwWindowShouldClose(window);}
        
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        
    private:
        /* data */
        void initWindow();
        
        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;   
    };        
}