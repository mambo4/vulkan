#pragma once

#include "m4_device.hpp"

// lib


// std
#include <string>

namespace m4
{
    class M4Texture
    {
    public:
        M4Texture(M4Device& device, const std::string& texturePath);
        ~M4Texture();

        M4Texture(const M4Texture&) = delete;
        M4Texture& operator=(const M4Texture&) = delete;

        M4Texture(M4Texture&&) = default;
        M4Texture& operator=(M4Texture&&) = default;

        VkImageView getImageView() const { return textureImageView; }
        VkSampler getSampler() const { return textureSampler; }
        VkImageLayout getImageLayout() const { return textureImageLayout; }

    private:
        void transitionImageLayout(VkImageLayout oldLayout,VkImageLayout newLayout);
        void generateMipmaps();
        
        int width;
        int height;     
        int mipLevels;
        
        M4Device& m4Device;


        VkImage textureImage;
        VkDeviceMemory textureDeviceMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
        VkFormat textureFormat;
        VkImageLayout textureImageLayout;
    };
}