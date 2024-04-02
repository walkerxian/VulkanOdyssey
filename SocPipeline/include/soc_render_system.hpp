#pragma once

#include "soc_device.hpp"
#include "soc_game_object.hpp"
#include "soc_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace soc{

    class SocRenderSystem
    {


    public:
        SocRenderSystem(SocDevice &device,VkRenderPass renderPass);
        ~SocRenderSystem();

        SocRenderSystem(const SocRenderSystem &) = delete;
        SocRenderSystem &operator=(const SocRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<SocGameObject> &gameObjects);

    private:
       
       void createPipelineLayout();
       void createPipeline(VkRenderPass renderPass);

        SocDevice &socDevice;

        std::unique_ptr<SocPipeline> socPipeline;
        VkPipelineLayout pipelineLayout;
    };
    
  
    

}