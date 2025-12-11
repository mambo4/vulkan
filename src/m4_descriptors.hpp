#pragma once

#include "m4_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace m4 {

class M4DescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(M4Device &m4Device) : m4Device{m4Device} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<M4DescriptorSetLayout> build() const;

   private:
    M4Device &m4Device;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  M4DescriptorSetLayout(
      M4Device &m4Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~M4DescriptorSetLayout();
  M4DescriptorSetLayout(const M4DescriptorSetLayout &) = delete;
  M4DescriptorSetLayout &operator=(const M4DescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  M4Device &m4Device;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class M4DescriptorWriter;
};

class M4DescriptorPool {
 public:
  class Builder {
   public:
    Builder(M4Device &m4Device) : m4Device{m4Device} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<M4DescriptorPool> build() const;

   private:
    M4Device &m4Device;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  M4DescriptorPool(
      M4Device &m4Device,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~M4DescriptorPool();
  M4DescriptorPool(const M4DescriptorPool &) = delete;
  M4DescriptorPool &operator=(const M4DescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  M4Device &m4Device;
  VkDescriptorPool descriptorPool;

  friend class M4DescriptorWriter;
};

class M4DescriptorWriter {
 public:
  M4DescriptorWriter(M4DescriptorSetLayout &setLayout, M4DescriptorPool &pool);

  M4DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  M4DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  M4DescriptorSetLayout &setLayout;
  M4DescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace m4