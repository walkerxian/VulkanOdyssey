#include "soc_app_base.hpp"

//std
#include <array>
#include <stdexcept>
#include<iostream>

namespace soc{

    SocAppBase::SocAppBase(){
        loadModels();
        createPipelineLayout();
        //createPipeline();
        recreateSwapChain();
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
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
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
        
        assert(socSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        //获取默认的管线配置
        SocPipeline::defaultPipelineConfigInfo(pipelineConfig);
        
        pipelineConfig.renderPass = socSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        //需要准备3份数据
        socPipeline = std::make_unique<SocPipeline>(
            socDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SocAppBase::recreateSwapChain()
    {
        auto extent = socWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = socWindow.getExtent();//重新获取窗口大小
            
            glfwWaitEvents();//当窗口最小化暂停？
        }

        //vkDeviceWaitIdle(socDevice.device);
        vkDeviceWaitIdle(socDevice.device());

        if(socSwapChain == nullptr){
            
            //std::cout<<"Start  "<<std::endl;
            //初始化的时候已经创建了命令缓冲区
            socSwapChain = std::make_unique<SocSwapChain>(socDevice,extent);

        }
        else{           
          // std::cout <<"Update  " << std::endl;
            socSwapChain = std::make_unique<SocSwapChain>(socDevice,extent,std::move(socSwapChain));
            if(socSwapChain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        createPipeline();    
        // std::cout<<"Create a pipeline!"<<std::endl;
        //throw std::runtime_error("Create a Pipeline");
    }

    //只负责命令缓冲区的分配，不再记录命令缓冲区
    void SocAppBase::createCommandBuffers(){
        
        commandBuffers.resize(socSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = socDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(socDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }        
    }

    void SocAppBase::recordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

              if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            //core 
            renderPassInfo.renderPass = socSwapChain->getRenderPass();
            renderPassInfo.framebuffer = socSwapChain->getFrameBuffer(imageIndex);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = socSwapChain->getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(socSwapChain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(socSwapChain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, socSwapChain->getSwapChainExtent()};
            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

            socPipeline->bind(commandBuffers[imageIndex]);
            //必须绑定
            socModel->bind(commandBuffers[imageIndex]);
            socModel->draw(commandBuffers[imageIndex]);

            vkCmdDraw(commandBuffers[imageIndex],3,1,0,0);
            vkCmdEndRenderPass(commandBuffers[imageIndex]);
           
            if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
                throw std::runtime_error("failed to record command buffer!");    
            }           
    }

    void SocAppBase::freeCommandBuffers(){
        vkFreeCommandBuffers(
            socDevice.device(),
            socDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());

        commandBuffers.clear();
    }

    void SocAppBase::drawFrame(){

        uint32_t imageIndex;
        auto result = socSwapChain->acquireNextImage(&imageIndex);
        
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

       //重新记录命令缓冲区，以至于缩放交换链之后重新绘制
        recordCommandBuffer(imageIndex);
      
        result = socSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
      
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||socWindow.wasWindowResized())//当重新调整窗口之后，会触发该事件
        {
           socWindow.resetWindowResizedFlag();

           //重建交换链只重建命令缓冲区，并不记录重新记录命令缓冲区；记
           //命令缓冲区的记录直接走正常的记录流程

           recreateSwapChain();

           return;
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
}