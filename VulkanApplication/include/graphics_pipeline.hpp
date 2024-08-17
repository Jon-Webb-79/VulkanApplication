// ================================================================================
// ================================================================================
// - File:    graphics_pipeline.hpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 30, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef graphics_pipeline_HPP
#define graphics_pipeline_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <array>
#include <memory>

#include "memory.hpp"
#include <vk_mem_alloc.h>

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

/**
 * @brief Manages the Vulkan graphics pipeline, including creation, command buffers, synchronization, and cleanup.
 *
 * The GraphicsPipeline class is responsible for setting up and managing the Vulkan graphics pipeline. This includes
 * creating the pipeline, render pass, framebuffers, command buffers, and synchronization objects.
 */
class GraphicsPipeline {
public:
    /**
     * @brief Constructs a GraphicsPipeline object and initializes the graphics pipeline.
     *
     * @param device The Vulkan logical device.
     * @param swapChainExtent The extent (dimensions) of the swap chain images.
     * @param swapChainImageFormat The format of the swap chain images.
     *
     * @throws std::runtime_error if the pipeline or render pass cannot be created.
     */
    GraphicsPipeline(VkDevice device, 
                     VkExtent2D swapChainExtent, 
                     VkFormat swapChainImageFormat,
                     VkPhysicalDevice physicalDevice,
                     VkQueue graphicsQue,
                     const std::vector<Vertex>& vertices,
                     const std::vector<uint16_t>& indices,
                     VkInstance& instance,
                     AllocatorManager& allocatorManager);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor for the GraphicsPipeline object.
     *
     * Cleans up the graphics pipeline, render pass, command pool, and synchronization objects.
     */
    ~GraphicsPipeline();
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan graphics pipeline.
     *
     * @return The Vulkan graphics pipeline (VkPipeline).
     */
    VkPipeline getPipeline() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the pipeline layout.
     *
     * @return The pipeline layout (VkPipelineLayout).
     */
    VkPipelineLayout getPipelineLayout() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the command buffer for a specific frame.
     *
     * @param frameIndex The index of the frame.
     * @return The command buffer (VkCommandBuffer) for the specified frame.
     */
    VkCommandBuffer getCommandBuffer(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the image available semaphore for a specific frame.
     *
     * @param frameIndex The index of the frame.
     * @return The image available semaphore (VkSemaphore) for the specified frame.
     */
    VkSemaphore getImageAvailableSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the render finished semaphore for a specific frame.
     *
     * @param frameIndex The index of the frame.
     * @return The render finished semaphore (VkSemaphore) for the specified frame.
     */
    VkSemaphore getRenderFinishedSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the in-flight fence for a specific frame.
     *
     * @param frameIndex The index of the frame.
     * @return The in-flight fence (VkFence) for the specified frame.
     */
    VkFence getInFlightFence(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the framebuffers for the swap chain images.
     *
     * @param swapChainImageViews The image views of the swap chain images.
     * @param swapChainExtent The extent (dimensions) of the swap chain images.
     *
     * @throws std::runtime_error if the framebuffers cannot be created.
     */
    void createFramebuffers(const std::vector<VkImageView>& swapChainImageViews, VkExtent2D swapChainExtent);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the framebuffers.
     *
     * @return A reference to a vector containing the framebuffers.
     */
    const std::vector<VkFramebuffer>& getFramebuffers() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the command pool for the graphics pipeline.
     *
     * @param physicalDevice The Vulkan physical device.
     * @param surface The Vulkan surface.
     *
     * @throws std::runtime_error if the command pool cannot be created.
     */
    void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the command buffers for the graphics pipeline.
     *
     * @throws std::runtime_error if the command buffers cannot be created.
     */
    void createCommandBuffers();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the synchronization objects for the graphics pipeline.
     *
     * @throws std::runtime_error if the synchronization objects cannot be created.
     */
    void createSyncObjects();
// --------------------------------------------------------------------------------

    /**
     * @brief Records the command buffer for a specific frame and image index.
     *
     * @param frameIndex The index of the frame.
     * @param imageIndex The index of the swap chain image.
     *
     * @throws std::runtime_error if the command buffer cannot be recorded.
     */
    void recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex);
// --------------------------------------------------------------------------------

    /**
     * @brief Waits for the fences of a specific frame.
     *
     * @param frameIndex The index of the frame.
     *
     * @throws std::runtime_error if waiting for the fence fails.
     */
    void waitForFences(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Resets the fences of a specific frame.
     *
     * @param frameIndex The index of the frame.
     *
     * @throws std::runtime_error if resetting the fence fails.
     */
    void resetFences(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
    * @brief Creates a vertex buffer and allocates memory for it.
    *
    * This method creates a Vulkan buffer to hold vertex data and allocates the necessary
    * memory for the buffer. The vertex data is then copied into the buffer.
    *
    * @throws std::runtime_error if the buffer or memory allocation fails.
    */
    void createVertexBuffer();
// --------------------------------------------------------------------------------

    /**
    * @brief Creates an index buffer and allocates memory for it.
    *
    * This method creates a Vulkan buffer to hold index data and allocates the necessary
    * memory for the buffer. The index data is then copied into the buffer.
    *
    * @throws std::runtime_error if the buffer or memory allocation fails.
    */
    void createIndexBuffer();
// --------------------------------------------------------------------------------

    /**
    * @brief Destroys all framebuffers associated with the pipeline.
    *
    * This method will be used to clean up the framebuffers when they are no longer needed,
    * such as when the swap chain is being recreated.
    */
    void destroyFramebuffers();
// --------------------------------------------------------------------------------

    VkCommandPool getCommandPool() const;
// --------------------------------------------------------------------------------

    VkExtent2D getSwapChainExtent() const;
// --------------------------------------------------------------------------------

    const std::vector<void*>& getUniformBuffersMapped() const;
// --------------------------------------------------------------------------------

    void createUniformBuffers();
// ================================================================================ 
private:
    VkDevice device;
    VkExtent2D swapChainExtent;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;


    VkPhysicalDevice physicalDevice;  // Physical device
    VkQueue graphicsQueue;
    std::vector<Vertex> vertices;  // Vertex data
    std::vector<uint16_t> indices;
    VkInstance instance;

    AllocatorManager& allocatorManager;
    VmaAllocation vertexBufferAllocation;
    VmaAllocation indexBufferAllocation;

    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VmaAllocation> uniformBuffersMemory;
    std::vector<VkBuffer> uniformBuffers;
 //   std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a shader module from the given shader code.
     *
     * @param code The shader code.
     * @return The shader module (VkShaderModule).
     *
     * @throws std::runtime_error if the shader module cannot be created.
     */
    VkShaderModule createShaderModule(const std::vector<char>& code);
// --------------------------------------------------------------------------------

    /**
     * @brief Reads a file and returns its contents as a vector of characters.
     *
     * @param filename The name of the file to read.
     * @return A vector containing the contents of the file.
     *
     * @throws std::runtime_error if the file cannot be read.
     */
    std::vector<char> readFile(const std::string& filename);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the graphics pipeline.
     *
     * @throws std::runtime_error if the graphics pipeline cannot be created.
     */
    void createGraphicsPipeline();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the render pass.
     *
     * @param swapChainImageFormat The format of the swap chain images.
     *
     * @throws std::runtime_error if the render pass cannot be created.
     */
    void createRenderPass(VkFormat swapChainImageFormat);
// --------------------------------------------------------------------------------

    /**
    * @brief Finds a suitable memory type for a Vulkan buffer.
    *
    * This method finds a suitable memory type for a Vulkan buffer based on the provided
    * type filter and memory property flags.
    *
    * @param typeFilter A bitmask representing the memory types that are suitable.
    * @param properties The desired memory property flags.
    * @return The index of a suitable memory type.
    *
    * @throws std::runtime_error if no suitable memory type is found.
    */
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// --------------------------------------------------------------------------------

    void createDescriptorSetLayout();
};
// ================================================================================
// ================================================================================
#endif /* graphics_pipeline_HPP */
