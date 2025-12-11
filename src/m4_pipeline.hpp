# pragma once

#include "m4_device.hpp"

# include <string>
# include <vector>
namespace m4 {

    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        PipelineConfigInfo() = default;
        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;

        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class M4Pipeline
    {
    public:
        M4Pipeline(
            M4Device& device, 
            const std::string& vertFilepath, 
            const std::string& fragFilepath , 
            const PipelineConfigInfo& configInfo );
        
        ~M4Pipeline();

        M4Pipeline(const M4Pipeline&) = delete;
        M4Pipeline& operator=(const M4Pipeline&) = delete;    

        void bind(VkCommandBuffer commandBuffer);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        
    private:
        static std::vector<char> readFile(const std::string& filepath);
        void createGraphicsPipeline( 
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
    
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        M4Device& m4Device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;

    };

 

}  // namespace m4