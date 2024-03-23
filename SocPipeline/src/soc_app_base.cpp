#include "soc_app_base.hpp"

//std
#include <array>
#include <stdexcept>

namespace soc{

    SocAppBase::SocAppBase(){
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    SocAppBase::~SocAppBase(){

        vkDestroyPipelineLayout(socDevice.device(),pipelineLayout,nullptr);
    }

    void SocAppBase::run(){

        while (!socWindow.shouldClose())
        {
            /* code */
            glfwPollEvents();

            drawFrame();
        }

        vkDeviceWaitIdle(socDevice.device());//why?
        
    }

void SocAppBase::loadModels() {
  std::vector<SocModel::Vertex> vertices{
      
      {{0.0f, -0.5f}},
      {{0.5f, 0.5f}},
      {{-0.5f, 0.5f}}
      };
  socModel = std::make_unique<SocModel>(socDevice, vertices);
}

    void SocAppBase::createPipelineLayout(){
              
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

          if (vkCreatePipelineLayout(socDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

    }

    void SocAppBase::createPipeline(){
        
        PipelineConfigInfo pipelineConfig{};
        //获取默认的管线配置
        SocPipeline::defaultPipelineConfigInfo(
            pipelineConfig,
            socSwapChain.width(),
            socSwapChain.height());
        
        pipelineConfig.renderPass = socSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        //需要准备3份数据
        socPipeline = std::make_unique<SocPipeline>(
            socDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SocAppBase::createCommandBuffers(){
        
        commandBuffers.resize(socSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = socDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(socDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (int i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

              if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            //core 
            renderPassInfo.renderPass = socSwapChain.getRenderPass();
            renderPassInfo.framebuffer = socSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = socSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            socPipeline->bind(commandBuffers[i]);
            socModel->bind(commandBuffers[i]);
            socModel->draw(commandBuffers[i]);

            vkCmdDraw(commandBuffers[i],3,1,0,0);
            vkCmdEndRenderPass(commandBuffers[i]);
           
            if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
                throw std::runtime_error("failed to record command buffer!");    
            }           
        }        
    }

    void SocAppBase::drawFrame(){

        uint32_t imageIndex;
        auto result = socSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        result = socSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
}