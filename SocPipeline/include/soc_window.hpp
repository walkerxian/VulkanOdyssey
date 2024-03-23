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

        VkExtent2D getExtent(){return {static_cast<uint32_t>(width),static_cast<uint32_t>(height)};}

        bool wasWindowResized(){return framebufferResized;}
        void resetWindowResizedFlag(){framebufferResized = false;}

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        
    private:

        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        /* data */
        void initWindow();
        
        const int width;
        const int height;
        bool framebufferResized = false;

        std::string windowName;
        GLFWwindow *window;   
    };        
}