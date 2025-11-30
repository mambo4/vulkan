# pragma once

#include "lbe_device.hpp"

# include <string>
# include <vector>
namespace lbe {

    struct PipelineConfigInfo {
        // Add pipeline configuration parameters as needed
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

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        
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