#pragma once

#include "soc_device.hpp"
#include "soc_game_object.hpp"
#include "soc_pipeline.hpp"
#include "soc_swap_chain.hpp"
#include "soc_window.hpp"
//std
#include <memory>


namespace soc{

    class SocAppBase{
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        static constexpr float ROTATION_SPEED = 0.00008f;

        SocAppBase();
        ~SocAppBase();


        SocAppBase(const SocAppBase &) = delete;
        SocAppBase& operator = (const SocAppBase &) = delete;
        
        void run();

        private:
        //void loadModels();
        void loadGameObjects();

        void createPipelineLayout();
        
        void createPipeline();
        
        //操作命令缓冲区： 创建，释放，记录
        void createCommandBuffers();
        void recordCommandBuffer(int imageIndex);
        void freeCommandBuffers();

        void renderGameObjects(VkCommandBuffer commandBuffer);

        //每帧绘制
        void drawFrame();
        void recreateSwapChain();
       
        //应用程序模块需要包含一下几部分：窗口系统，设备系统，交换链（绘制到窗口），管线（提交命令缓冲区中）
        //CommandBuffer命令缓冲区，需要绘制的物体SocGameObject

        SocWindow socWindow{WIDTH,HEIGHT,"Hello Vulkan!"};        
        SocDevice socDevice{socWindow};
        //SocSwapChain socSwapChain{socDevice,socWindow.getExtent()};
        std::unique_ptr<SocSwapChain> socSwapChain;
        std::unique_ptr<SocPipeline> socPipeline;        
        VkPipelineLayout pipelineLayout;                 
        std::vector<VkCommandBuffer> commandBuffers;
        //std::unique_ptr<SocModel> socModel;
        std::vector<SocGameObject> gameObjects;
        // SocPipeline pipeline{
        //     socDevice,
        //     "shaders\\simple_shader.vert.spv", 
        //     "shaders\\simple_shader.frag.spv",
        //     SocPipeline::defaultPipelineConfigInfo(WIDTH,HEIGHT)};
    };
}