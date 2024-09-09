// ================================================================================
// ================================================================================
// - File:    graphics.hpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 30, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef graphics_HPP
#define graphics_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "memory.hpp"
#include "devices.hpp"
#include <vk_mem_alloc.h>
// ================================================================================
// ================================================================================ 

const int MAX_FRAMES_IN_FLIGHT = 2;
// ================================================================================
// ================================================================================ 


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};
// ================================================================================
// ================================================================================


struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
// ================================================================================
// ================================================================================


class CommandBufferManager {
public:
    CommandBufferManager(VkDevice device,
                         const std::vector<uint16_t>& indices,
                         VkPhysicalDevice physicalDevice,
                         VkSurfaceKHR surface);
// --------------------------------------------------------------------------------

    ~CommandBufferManager();
// --------------------------------------------------------------------------------

    void waitForFences(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    void resetFences(uint32_t) const;
// --------------------------------------------------------------------------------

    const VkCommandPool& getCommandPool() const;
// --------------------------------------------------------------------------------

    const std::vector<VkCommandBuffer>& getCommandBuffers() const;
// --------------------------------------------------------------------------------

    const VkCommandBuffer& getCommandBuffer(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    const VkSemaphore& getImageAvailableSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    const VkSemaphore& getRenderFinishedSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    const VkFence& getInFlightFence(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    void createCommandBuffers();
// ================================================================================
private:
    // Attributes passed to constructor
    VkDevice device;
    VkExtent2D swapChainExtent;
    std::vector<uint16_t> indices;

    // Attributes created in class
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
// --------------------------------------------------------------------------------

    void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
// --------------------------------------------------------------------------------

    void createSyncObjects();
};
// ================================================================================
// ================================================================================


class BufferManager {
public:
    BufferManager(const std::vector<Vertex>& vertices,
                  const std::vector<uint16_t>& indices,
                  AllocatorManager& allocatorManager,
                  CommandBufferManager& commandBufferManager,
                  VkQueue graphicsQueue);
// --------------------------------------------------------------------------------

    ~BufferManager();
// --------------------------------------------------------------------------------

    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
// --------------------------------------------------------------------------------

    const VkBuffer getVertexBuffer() const;
// --------------------------------------------------------------------------------

    const VkBuffer getIndexBuffer() const;
// --------------------------------------------------------------------------------

    const std::vector<VkBuffer>& getUniformBuffers() const;
// --------------------------------------------------------------------------------

    const std::vector<void*>& getUniformBuffersMapped() const;
// ================================================================================
private:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    AllocatorManager& allocatorManager;
    CommandBufferManager& commandBufferManager;
    VkQueue graphicsQueue;

    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VmaAllocation vertexBufferAllocation;
    VmaAllocation indexBufferAllocation;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;
    std::vector<VmaAllocation> uniformBuffersMemory;
// --------------------------------------------------------------------------------

    bool createVertexBuffer();
// --------------------------------------------------------------------------------

    bool createIndexBuffer();
// --------------------------------------------------------------------------------

    bool createUniformBuffers();
};
// // ================================================================================
// // ================================================================================ 
//
//
class DescriptorManager {
public:

    DescriptorManager(VkDevice device);
// --------------------------------------------------------------------------------

    ~DescriptorManager();
// --------------------------------------------------------------------------------

    void createDescriptorSets(const std::vector<VkBuffer> uniformBuffers);
// --------------------------------------------------------------------------------

    const VkDescriptorSetLayout& getDescriptorSetLayout() const;
// --------------------------------------------------------------------------------

    const VkDescriptorPool& getDescriptorPool() const;
// --------------------------------------------------------------------------------

    const std::vector<VkDescriptorSet>& getDescriptorSets() const;
// --------------------------------------------------------------------------------

    const VkDescriptorSet& getDescriptorSet(uint32_t frameIndex) const;
// ================================================================================
private:
    VkDevice device;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
// --------------------------------------------------------------------------------

    void createDescriptorSetLayout();
// --------------------------------------------------------------------------------

    void createDescriptorPool();
};
// ================================================================================
// ================================================================================ 

class GraphicsPipeline {
public:
    GraphicsPipeline(VkDevice device,
                     SwapChain& swapChain,
                     CommandBufferManager& commandBufferManager,
                     BufferManager& bufferManager,
                     DescriptorManager& descirptorManager,
                     const std::vector<uint16_t>& indices,
                     VkPhysicalDevice physicalDevice);
    // GraphicsPipeline(VkDevice device,
    //                  VkExtent2D swapChainExtent,
    //                  std::vector<VkCommandBuffer>& commandBuffers,
    //                  VkBuffer vertexBuffer,
    //                  VkBuffer indexBuffer,
    //                  std::vector<uint16_t>& indices,
    //                  std::vector<VkDescriptorSet>& descriptorSets,
    //                  VkFormat swapChainImageFormat,
    //                  VkPhysicalDevice physicalDevice,
    //                  VkDescriptorSetLayout descriptorSetlayout);
// --------------------------------------------------------------------------------

    ~GraphicsPipeline();
// --------------------------------------------------------------------------------

    void createFrameBuffers(const std::vector<VkImageView>& swapChainImageViews, 
                            VkExtent2D swapChainExtent);
// --------------------------------------------------------------------------------

    void destroyFramebuffers();
// --------------------------------------------------------------------------------

    void recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex);
// --------------------------------------------------------------------------------

    const VkPipelineLayout& getPipelineLayout() const;
// --------------------------------------------------------------------------------

    const VkPipeline& getPipeline() const;
// --------------------------------------------------------------------------------

    const VkRenderPass& getRenderPass() const;
// --------------------------------------------------------------------------------

    const std::vector<VkFramebuffer>& getFrameBuffers() const;
// --------------------------------------------------------------------------------

    const VkFramebuffer& getFrameBuffer(uint32_t frameIndex) const;
// ================================================================================
private:
    VkDevice device;
    SwapChain& swapChain;
    CommandBufferManager& commandBufferManager;
    BufferManager& bufferManager;
    DescriptorManager& descriptorManager;
    std::vector<uint16_t> indices;
    VkPhysicalDevice physicalDevice;

    // VkExtent2D swapChainExtent;
    // std::vector<VkCommandBuffer> commandBuffers;
    // VkBuffer vertexBuffer;
    // VkBuffer indexBuffer;
    // const std::vector<uint16_t> indices;
    // std::vector<VkDescriptorSet> descriptorSets;
    // VkPhysicalDevice physicalDevice;
    // VkDescriptorSetLayout descriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;
// --------------------------------------------------------------------------------

    VkShaderModule createShaderModule(const std::vector<char>& code);
// --------------------------------------------------------------------------------

    std::vector<char> readFile(const std::string& filename);
// --------------------------------------------------------------------------------

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// --------------------------------------------------------------------------------

    void createRenderPass(VkFormat swapChainImageFormat);
// --------------------------------------------------------------------------------

    void createGraphicsPipeline();
};
// ================================================================================
// ================================================================================
#endif /* graphics_HPP */
// ================================================================================
// ================================================================================
// eof
