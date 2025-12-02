# pragma once

#include "lbe_device.hpp"

# include <string>
# include <vector>
namespace lbe {

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

    class LbePipeline
    {
    public:
        LbePipeline(
            LbeDevice& device, 
            const std::string& vertFilepath, 
            const std::string& fragFilepath , 
            const PipelineConfigInfo& configInfo );
        
        ~LbePipeline();

        LbePipeline(const LbePipeline&) = delete;
        LbePipeline& operator=(const LbePipeline&) = delete;    

        // static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);
        
    private:
        static std::vector<char> readFile(const std::string& filepath);
        void createGraphicsPipeline( 
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
    
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        LbeDevice& lbeDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;

    };



}  // namespace lbe