#include "soc_renderer.hpp"


//std
#include <array>
#include <cassert>
#include <stdexcept>

namespace soc
{
    SocRenderer::SocRenderer(SocWindow& window,SocDevice& device)
    : socWindow{window},socDevice{device}{
        
        recreateSwapChain();
        createCommandBuffers();
    }

    SocRenderer::~SocRenderer(){freeCommandBuffers();}


    void SocRenderer::recreateSwapChain(){

    }
  
     //vkBeginCommandBuffer为命令的记录做好了准备，开发者接下来就可以向命令缓冲区中记录绘制命令，
    //状态改变命令和其他GPU操作命令了，一旦完成命令的记录就可以通过vkEndCommandBuffer来结束记录过程，
    //并最终将命令缓冲区提交给队列执行   

    void SocRenderer::createCommandBuffers(){
        //表示多帧处理，每个commandBuffer对一个frame处理
        commandBuffers.resize(SocSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = socDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(socDevice.device(),&allocInfo,commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers!");
        }        
    }

    VkCommandBuffer SocRenderer::beginFrame(){

        assert(!isFrameStarted && "Can't call beginFrame while already in progress");
        //需要从SwapChain里面 Get vkAcquireNextImageKHR
        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            //表示没有 swap chian image
            throw std::runtime_error("failed to acquire swap chain image!");            
        }

        //表示成功获取到了SwapChain Image
        //SwapChian Image? ImageView?还是Attachment?
        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer,&beginInfo) != VK_SUCCESS){
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        return commandBuffer;
    }


    void SocRenderer::endFrame(){

        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
            throw std::runtime_error("failed to record command buffer!");
        }
        //为什么不是重建交换链之后再提交？

        //记录完成之后，开始提交
       auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        //更新过交换链之后
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        socWindow.wasWindowResized()) {
            
            socWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
        //涉及到交换链中的多重缓冲
        currentFrameIndex = (currentFrameIndex + 1) % SocSwapChain::MAX_FRAMES_IN_FLIGHT;

    }
    
   

    void SocRenderer::freeCommandBuffers(){

        vkFreeCommandBuffers(
            socDevice.device(),
            socDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());

        commandBuffers.clear();
    }


 

    //精确描述了一系列渲染操作的集合，以及这些操作如何与帧缓冲区中的各种附件Attachment交互；
    void SocRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
        //断言是不满足条件则执行
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&"Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = socSwapChain->getRenderPass();
        //为renderPass指定具体的资源
        renderPassInfo.framebuffer = socSwapChain->getFrameBuffer(currentFrameIndex);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = socSwapChain->getSwapChainExtent();

        std::array<VkClearValue,2> clearValues{};
        clearValues[0].color = {0.01f,0.01f,0.01f,1.0f};
        clearValues[1].depthStencil = {1.0f,0};

        //指定渲染操作的信息
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer,&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        //处理渲染视口
        viewport.width = static_cast<float>(socSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(socSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        //offset + extent
        VkRect2D scissor{{0,0},socSwapChain->getSwapChainExtent()};
        
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    //如何在Vulkan中安全地结束一个渲染通道。
    void SocRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){            
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");        
        vkCmdEndRenderPass(commandBuffers);
    }
    
    
}