#include "soc_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

  
// std
#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>


namespace soc{

    struct SimplePushConstantData {
    glm::mat2 transform{1.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
    };
    
    SocRenderSystem::SocRenderSystem(SocDevice& device, VkRenderPass renderPass)
    : socDevice{device} {
    
        createPipelineLayout();
        createPipeline(renderPass);
    
    }

    SocRenderSystem::~SocRenderSystem(){

        vkDestroyPipelineLayout(socDevice.device(),pipelineLayout,nullptr);
    }

    void SocRenderSystem::createPipelineLayout(){

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(socDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SocRenderSystem::createPipeline(VkRenderPass renderPass){
        //必须要创建pipelineLayout才可以创建管线
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        SocPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        socPipeline = std::make_unique<SocPipeline>(
            socDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    //绘制操作都在RenderSystem里面;不同的渲染系统负责自己的渲染逻辑
    void SocRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer,
        std::vector<SocGameObject>& gameObjects){
        //先绑定
        socPipeline->bind(commandBuffer);
        for (auto& obj : gameObjects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.0001f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);                
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }    
    }

}
  
