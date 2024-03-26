#pragma once

#include "soc_device.hpp"

#include <string>
#include <vector>

namespace soc{

    struct PipelineConfigInfo {

        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkViewport viewport;
        VkRect2D scissor;
        
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };


    class SocPipeline
    {
 
    public:
        SocPipeline(
            SocDevice& device,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);

        ~SocPipeline();

        SocPipeline(const SocPipeline&) = delete;
        void operator=(const SocPipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void  defaultPipelineConfigInfo(
            PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string& filename);      
        
        void createGraphicsPipeline(
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);    

        void createShaderModule(const std::vector<char>& code,VkShaderModule* shaderModule);

        SocDevice& socDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
    
}