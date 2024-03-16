#include "soc_app_base.hpp"

namespace soc{
    void SocAppBase::run(){

        while (!socWindow.shouldClose())
        {
            /* code */
            glfwPollEvents();
        }
        
    }
}