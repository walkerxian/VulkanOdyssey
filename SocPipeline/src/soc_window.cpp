
#include "soc_window.hpp"

//std
#include <stdexcept>

namespace soc
{

   SocWindow::SocWindow(int w,int h,std::string name):width(w),height(h),windowName(name)
    {
        initWindow();
    }
    

    SocWindow::~SocWindow(){

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    //初始化窗口系统
    void SocWindow::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);//可以缩放窗口大小

        window = glfwCreateWindow(width,height,windowName.c_str(),nullptr,nullptr);
        //允许我们将GLFW窗口对象与任意指针配对
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }
    /// @brief 创建WindowSurface
    /// @param instance Vulkan Instance
    /// @param surface   
    void SocWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {       
            throw std::runtime_error("failed to create window surface!");
        }
    }

    //调整FrameBuffer Size大小： FrameBuffer是包含一系列ImageView 
    //具体包含FrameBuffer是由 RenderPass来指定的，比如ColorAttachement,DepthAttachment等
    void SocWindow::framebufferResizeCallback(GLFWwindow * window,int width,int height)
    {
        auto  socWindow = reinterpret_cast<SocWindow *>(glfwGetWindowUserPointer(window));
        socWindow->framebufferResized = true;
        socWindow->width = width;
        socWindow->height = height;
    }


} // namespace soc


