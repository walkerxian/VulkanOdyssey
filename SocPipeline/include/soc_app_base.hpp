#pragma once

#include "soc_window.hpp"
#include "soc_pipeline.hpp"
#include "soc_swap_chain.hpp"
#include "soc_model.hpp"

//std
#include <memory>


namespace soc{

    class SocAppBase{
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        SocAppBase();
        ~SocAppBase();


        SocAppBase(const SocAppBase &) = delete;
        SocAppBase& operator = (const SocAppBase &) = delete;
        
        void run();

        private:
        void loadModels();

        void createPipelineLayout();
        
        void createPipeline();
        
        void createCommandBuffers();
        void recordCommandBuffer(int imageIndex);
        void freeCommandBuffers();

        void drawFrame();
        void recreateSwapChain();
       

        SocWindow socWindow{WIDTH,HEIGHT,"Hello Vulkan!"};        
        SocDevice socDevice{socWindow};
        //SocSwapChain socSwapChain{socDevice,socWindow.getExtent()};
        std::unique_ptr<SocSwapChain> socSwapChain;

        std::unique_ptr<SocPipeline> socPipeline;        
        VkPipelineLayout pipelineLayout;         
        std::vector<VkCommandBuffer> commandBuffers;

        std::unique_ptr<SocModel> socModel;

        // SocPipeline pipeline{
        //     socDevice,
        //     "shaders\\simple_shader.vert.spv", 
        //     "shaders\\simple_shader.frag.spv",
        //     SocPipeline::defaultPipelineConfigInfo(WIDTH,HEIGHT)};
    };
}