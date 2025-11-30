#include "lbe_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace lbe {

    LbePipeline::LbePipeline(const std::string& vertFilepath, const std::string& fragFilepath) {
        createGraphicsPipeline(vertFilepath, fragFilepath);
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

    void LbePipeline::createGraphicsPipeline( const std::string& vertFilepath, const std::string& fragFilepath) {
        auto vertShaderCode = readFile(vertFilepath);
        auto fragShaderCode = readFile(fragFilepath);
        
        std::cout << "Vertex shader code size: " << vertShaderCode.size() << " bytes\n";
        std::cout << "Fragment shader code size: " << fragShaderCode.size() << " bytes\n";
    }   

}