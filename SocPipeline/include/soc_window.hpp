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
        SocWindow& operation(const SocWindow &) = delete;
        
        //是否需要关闭窗口系统
        bool shouldClose(){return glfwWindowShouldClose(window);}        

        VkExtent2D getExtent(){return {static_cast<uint32_t>(width),static_cast<uint32_t>(height)};}

        //是否需要重新调整窗口大小-》重新调整交换链的大小
        bool wasWindowResized(){return framebufferResized;}
        void resetWindowResizedFlag(){framebufferResized = false;}

        //创建WindowSurface
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        
    private:

        //新的宽、高
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        /* data */
        void initWindow();
        
        int width;
        int height;
        bool framebufferResized = false;

        std::string windowName;
        GLFWwindow *window;   
    };        
}