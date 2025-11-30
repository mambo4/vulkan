#include "lbe_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace lbe {

    LbePipeline::LbePipeline(
            LbeDevice& device, 
            const std::string& vertFilepath, 
            const std::string& fragFilepath , 
            const PipelineConfigInfo& configInfo ) : lbeDevice{device} {
        createGraphicsPipeline(vertFilepath, fragFilepath,configInfo);
    }

    LbePipeline::~LbePipeline() {
        vkDestroyShaderModule(lbeDevice.device(), fragShaderModule, nullptr);
        vkDestroyShaderModule(lbeDevice.device(), vertShaderModule, nullptr);
        vkDestroyPipeline(lbeDevice.device(), graphicsPipeline, nullptr);
    }

    std::vector<char> LbePipeline::readFile(const std::string& filepath) {
        
        std::ifstream file{filepath, std::ios::ate |  std::ios::binary};
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filepath);
        }
        size_t fileSize = static_cast<size_t>(file.tellg()); 
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer; 
    }

    void LbePipeline::createGraphicsPipeline( const std::string& vertFilepath, const std::string& fragFilepath,const PipelineConfigInfo& configInfo) {
        auto vertShaderCode = readFile(vertFilepath);
        auto fragShaderCode = readFile(fragFilepath);
        
        std::cout << "Vertex shader code size: " << vertShaderCode.size() << " bytes\n";
        std::cout << "Fragment shader code size: " << fragShaderCode.size() << " bytes\n";
    }   

    void LbePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule){
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(lbeDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        } 
    }

    PipelineConfigInfo LbePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height){
        PipelineConfigInfo configInfo{};
        // Set default configuration parameters as needed
        return configInfo;
    }
        


}