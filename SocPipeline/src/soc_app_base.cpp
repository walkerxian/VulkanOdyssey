#include "soc_app_base.hpp"

#include "soc_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>



//std
#include <array>
#include <stdexcept>
#include <cassert>

namespace soc{


    // struct SimplePushConstantData {
    //     glm::mat2 transform{1.f};//初始化单位阵
    //     glm::vec2 offset;
    //     alignas(16) glm::vec3 color;
    // };


    SocAppBase::SocAppBase(){
        //loadModels();
        loadGameObjects();
        // createPipelineLayout();
        // //createPipeline();
        // recreateSwapChain();
        // createCommandBuffers();
    }

    SocAppBase::~SocAppBase(){

       // vkDestroyPipelineLayout(socDevice.device(),pipelineLayout,nullptr);
    }

    void SocAppBase::run(){

        SocRenderSystem socRenderSystem{socDevice,socRenderer.getSwapChainRenderPass()};

        //std::cout<<"maxPushConstantSize = " << socDevice.properties.limits.maxPushConstantsSize<<"\n";
        while (!socWindow.shouldClose())
        {
            /* code */
            glfwPollEvents();

           // drawFrame();
           if (auto commandBuffer = socRenderer.beginFrame()) {
      
            socRenderer.beginSwapChainRenderPass(commandBuffer);

            socRenderSystem.renderGameObjects(commandBuffer, gameObjects);

            socRenderer.endSwapChainRenderPass(commandBuffer);

            socRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(socDevice.device());//why?
        
    }

    //自定义模型数据和矩阵数据（平移，旋转，缩放）
    void SocAppBase::loadGameObjects() {

        std::vector<SocModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

        auto socModel = std::make_shared<SocModel>(socDevice, vertices);
        auto triangle = SocGameObject::createGameObject();
        
        triangle.model = socModel;
        
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        //triangle.transform2d.scale = {1.f, 1.f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();
        //这里只压了一个三角形啊
        gameObjects.push_back(std::move(triangle));

        //std::cout<<"Size is " << gameObjects.size()<<std::endl;
    }
/*
    //创建管线Layout数据
    void SocAppBase::createPipelineLayout(){
        
        //1 Push Constant Range数据
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        //2 PipelineLayout数据
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges =&pushConstantRange;

        if (vkCreatePipelineLayout(socDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SocAppBase::createPipeline(){
        
        assert(socSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        //1 设置Default Pipeline ConfigInfo
        SocPipeline::defaultPipelineConfigInfo(pipelineConfig);
        //2 设置RenderPass
        pipelineConfig.renderPass = socSwapChain->getRenderPass();
        //3 设置PipeineLayout
        pipelineConfig.pipelineLayout = pipelineLayout;      

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
        vkDeviceWaitIdle(socDevice.device());//重新创建交换链的时候，需要等待当前的交换链失效才能创建

        if(socSwapChain == nullptr){
                       
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

        //创建图形管线
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

    //负责记录命令到Vulkan命令缓冲区，这些命令将用于渲染imageIndex相关联的给定帧
    void SocAppBase::recordCommandBuffer(int imageIndex)
    {
        // static int frame = 0;
        // frame = (frame + 1) % 100;
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        //准备工作，开始记录，如果准备工作失败，则抛出异常
        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        //
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

        renderGameObjects(commandBuffers[imageIndex]);



        // socPipeline->bind(commandBuffers[imageIndex]);       
        // socModel->bind(commandBuffers[imageIndex]);

        // //Push ConstantRange
        // for (int j = 0; j < 4; j++) {
        //     SimplePushConstantData push{};
        //     push.offset = {-0.5f + frame * 0.02f, -0.4f + j * 0.25f};
        //     push.color = {0.0f, 0.0f, 0.2f + 0.2f * j};

        //     vkCmdPushConstants(
        //         commandBuffers[imageIndex],
        //         pipelineLayout,
        //         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        //         0,
        //         sizeof(SimplePushConstantData),
        //         &push);
        //     socModel->draw(commandBuffers[imageIndex]);
        // }

        // vkCmdDraw(commandBuffers[imageIndex],3,1,0,0);
        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        
        if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
            throw std::runtime_error("failed to record command buffer!");    
        }           
    }

    void SocAppBase::renderGameObjects(VkCommandBuffer commandBuffer)
    {
        socPipeline->bind(commandBuffer);

        for (auto& obj : gameObjects)
        {
            //先计算旋转
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + ROTATION_SPEED,glm::two_pi<float>());
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;//当前Shader着色器并没有使用该push constant 
            push.transform = obj.transform2d.mat2();

            //通过常量推送进行更新矩阵
           vkCmdPushConstants(commandBuffer,pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,0,sizeof(SimplePushConstantData),&push);

           obj.model->bind(commandBuffer);

           obj.model->draw(commandBuffer);
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

*/

}