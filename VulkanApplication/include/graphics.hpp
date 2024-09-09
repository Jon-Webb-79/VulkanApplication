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


/**
 * @brief Represents a vertex with position and color attributes.
 *
 * This struct defines a vertex with a 2D position and a 3D color. It also provides
 * static methods to describe how these vertex attributes are laid out in memory
 * for Vulkan's vertex input system. 
 */
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
// --------------------------------------------------------------------------------

    /**
     * @brief Returns the binding description for the vertex input.
     *
     * This function specifies how the vertex data is organized in the vertex buffer.
     * It provides the binding index, the byte stride between consecutive vertex data,
     * and the rate at which the input should advance.
     * 
     * @return A VkVertexInputBindingDescription struct that describes the input binding.
     */
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
// --------------------------------------------------------------------------------

    /**
     * @brief Returns the attribute descriptions for the vertex input.
     *
     * This function describes the vertex attributes (position and color) and their
     * layout in memory. It specifies the format of each attribute and the byte offset
     * from the start of the vertex structure.
     * 
     * @return A std::array of VkVertexInputAttributeDescription structs that describe the vertex attributes.
     */
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
 * @class CommandBufferManager
 * @brief Manages the creation, allocation, and synchronization of command buffers, fences, and semaphores for Vulkan.
 *
 * This class encapsulates the command buffer management in Vulkan, providing methods for creating command pools,
 * allocating command buffers, and managing synchronization primitives like fences and semaphores.
 */
class CommandBufferManager {
public:
    
    /**
     * @brief Constructor for CommandBufferManager.
     *
     * @param device The Vulkan device handle used for command buffer and resource creation.
     * @param indices The vector of indices used for managing the command buffers.
     * @param physicalDevice The Vulkan physical device used to create the command pool.
     * @param surface The Vulkan surface handle used for surface-related operations.
     */
    CommandBufferManager(VkDevice device,
                         const std::vector<uint16_t>& indices,
                         VkPhysicalDevice physicalDevice,
                         VkSurfaceKHR surface);
// --------------------------------------------------------------------------------
    
    /**
     * @brief Destructor for CommandBufferManager. Cleans up resources like command buffers, semaphores, and fences.
     */
    ~CommandBufferManager();
// --------------------------------------------------------------------------------
    
    /**
     * @brief Waits for a specific frame's fences to be signaled before proceeding.
     *
     * @param frameIndex The index of the frame whose fence should be waited for.
     */
    void waitForFences(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Resets the fences for a specific frame, allowing it to be reused.
     *
     * @param frameIndex The index of the frame whose fence should be reset.
     */
    void resetFences(uint32_t) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan command pool associated with this manager.
     *
     * @return The Vulkan command pool.
     */
    const VkCommandPool& getCommandPool() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of command buffers managed by this class.
     *
     * @return A reference to the vector of Vulkan command buffers.
     */
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;
// --------------------------------------------------------------------------------
    
    /**
     * @brief Retrieves a specific command buffer for a given frame index.
     *
     * @param frameIndex The index of the frame for which the command buffer is retrieved.
     * @return The Vulkan command buffer for the specified frame.
     */
    const VkCommandBuffer& getCommandBuffer(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the image available semaphore for a given frame index.
     *
     * @param frameIndex The index of the frame for which the image available semaphore is retrieved.
     * @return The Vulkan semaphore for signaling image availability.
     */
    const VkSemaphore& getImageAvailableSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the render finished semaphore for a given frame index.
     *
     * @param frameIndex The index of the frame for which the render finished semaphore is retrieved.
     * @return The Vulkan semaphore for signaling when rendering is finished.
     */
    const VkSemaphore& getRenderFinishedSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the in-flight fence for a given frame index.
     *
     * @param frameIndex The index of the frame for which the in-flight fence is retrieved.
     * @return The Vulkan fence for synchronization of frame rendering.
     */
    const VkFence& getInFlightFence(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Allocates and creates the command buffers for rendering.
     */
    void createCommandBuffers();
// ================================================================================
private:
    // Attributes passed to constructor
    VkDevice device;                      /**< The Vulkan device handle. */
    VkExtent2D swapChainExtent;           /**< The extent of the swap chain for rendering. */
    std::vector<uint16_t> indices;        /**< Vector holding index data for command buffers. */

    VkCommandPool commandPool = VK_NULL_HANDLE; /**< The Vulkan command pool used to allocate command buffers. */
    std::vector<VkCommandBuffer> commandBuffers; /**< The list of Vulkan command buffers. */
    std::vector<VkSemaphore> imageAvailableSemaphores; /**< Semaphores used to signal when images are available. */
    std::vector<VkSemaphore> renderFinishedSemaphores; /**< Semaphores used to signal when rendering is finished. */
    std::vector<VkFence> inFlightFences; /**< Fences used for synchronizing frame rendering. */ 
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a command pool for allocating command buffers.
     *
     * @param physicalDevice The Vulkan physical device used to create the command pool.
     * @param surface The Vulkan surface handle used for surface-related operations.
     */
    void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates synchronization objects like semaphores and fences for rendering.
     */
    void createSyncObjects();
};
// ================================================================================
// ================================================================================

/**
 * @class BufferManager
 * @brief Manages vertex, index, and uniform buffers for Vulkan rendering.
 *
 * This class encapsulates the management and allocation of various Vulkan buffers, such as vertex buffers,
 * index buffers, and uniform buffers. It also handles the mapping and updating of uniform buffers for different
 * frames.
 */
class BufferManager {
public:
     /**
     * @brief Constructor for BufferManager.
     *
     * @param vertices A vector of Vertex objects representing the vertex data.
     * @param indices A vector of 16-bit unsigned integers representing the index data.
     * @param allocatorManager A reference to the AllocatorManager responsible for memory allocation.
     * @param commandBufferManager A reference to the CommandBufferManager used for command buffer management.
     * @param graphicsQueue The Vulkan queue used for submitting graphics commands.
     */
    BufferManager(const std::vector<Vertex>& vertices,
                  const std::vector<uint16_t>& indices,
                  AllocatorManager& allocatorManager,
                  CommandBufferManager& commandBufferManager,
                  VkQueue graphicsQueue);
// --------------------------------------------------------------------------------
    
    /**
     * @brief Destructor for BufferManager.
     *
     * Cleans up all allocated Vulkan buffers and frees any associated memory.
     */
    ~BufferManager();
// --------------------------------------------------------------------------------

    /**
     * @brief Updates the uniform buffer with new data for the current frame.
     *
     * @param currentFrame The index of the current frame.
     * @param ubo The UniformBufferObject containing the new data to be copied to the uniform buffer.
     */
    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan vertex buffer.
     *
     * @return The Vulkan buffer used for storing vertex data.
     */
    const VkBuffer getVertexBuffer() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan index buffer.
     *
     * @return The Vulkan buffer used for storing index data.
     */
    const VkBuffer getIndexBuffer() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of uniform buffers used for each frame.
     *
     * @return A reference to the vector of Vulkan uniform buffers.
     */
    const std::vector<VkBuffer>& getUniformBuffers() const;
// --------------------------------------------------------------------------------
    
    /**
     * @brief Retrieves the vector of mapped uniform buffers used for memory access.
     *
     * @return A reference to the vector of void pointers that map the uniform buffers.
     */
    const std::vector<void*>& getUniformBuffersMapped() const;
// ================================================================================
private:
    std::vector<Vertex> vertices;                   /**< The vertex data used for rendering. */
    std::vector<uint16_t> indices;                  /**< The index data for drawing elements. */
    AllocatorManager& allocatorManager;             /**< The memory allocator manager for handling buffer memory. */
    CommandBufferManager& commandBufferManager;     /**< Command buffer manager for managing related command buffers. */
    VkQueue graphicsQueue;                          /**< The Vulkan queue used for submitting graphics commands. */

    VkBuffer vertexBuffer;                          /**< Vulkan buffer for storing vertex data. */
    VkBuffer indexBuffer;                           /**< Vulkan buffer for storing index data. */
    VmaAllocation vertexBufferAllocation;           /**< Memory allocation handle for the vertex buffer. */
    VmaAllocation indexBufferAllocation;            /**< Memory allocation handle for the index buffer. */

    std::vector<VkBuffer> uniformBuffers;           /**< Vector of Vulkan buffers used for uniform data across frames. */
    std::vector<void*> uniformBuffersMapped;        /**< Vector of pointers that map uniform buffers for direct memory access. */
    std::vector<VmaAllocation> uniformBuffersMemory;/**< Memory allocation handles for the uniform buffers. */
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the vertex buffer and allocates memory for it.
     *
     * @return True if the vertex buffer was successfully created, false otherwise.
     */
    bool createVertexBuffer();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the index buffer and allocates memory for it.
     *
     * @return True if the index buffer was successfully created, false otherwise.
     */
    bool createIndexBuffer();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates uniform buffers for each frame in the application.
     *
     * @return True if the uniform buffers were successfully created, false otherwise.
     */
    bool createUniformBuffers();
};
// // ================================================================================
// // ================================================================================ 

/**
 * @class DescriptorManager
 * @brief Manages Vulkan descriptor sets, layouts, and descriptor pools.
 *
 * This class handles the creation and management of Vulkan descriptor sets, layouts, and descriptor pools.
 * It is responsible for creating the descriptor sets for each frame and managing the descriptor pool and layout.
 */
class DescriptorManager {
public:
    /**
     * @brief Constructor for DescriptorManager.
     *
     * @param device The Vulkan device handle used for creating descriptor sets and pools.
     */
    DescriptorManager(VkDevice device);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor for DescriptorManager.
     *
     * Cleans up the descriptor pool and descriptor set layout associated with the manager.
     */
    ~DescriptorManager();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates descriptor sets for each frame in the application.
     *
     * @param uniformBuffers A vector of Vulkan buffers that hold the uniform buffer data for each frame.
     */
    void createDescriptorSets(const std::vector<VkBuffer> uniformBuffers);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan descriptor set layout.
     *
     * @return A reference to the Vulkan descriptor set layout.
     */
    const VkDescriptorSetLayout& getDescriptorSetLayout() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan descriptor pool.
     *
     * @return A reference to the Vulkan descriptor pool.
     */
    const VkDescriptorPool& getDescriptorPool() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of Vulkan descriptor sets.
     *
     * @return A reference to the vector of Vulkan descriptor sets.
     */
    const std::vector<VkDescriptorSet>& getDescriptorSets() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the descriptor set for a specific frame.
     *
     * @param frameIndex The index of the frame for which to retrieve the descriptor set.
     * @return A reference to the Vulkan descriptor set for the given frame.
     */
    const VkDescriptorSet& getDescriptorSet(uint32_t frameIndex) const;
// ================================================================================
private:
    VkDevice device;                                /**< The Vulkan device handle. */

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;  /**< The layout of the descriptor sets. */
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;            /**< The descriptor pool for allocating descriptor sets. */
    std::vector<VkDescriptorSet> descriptorSets;                 /**< A vector holding descriptor sets for each frame. */
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the Vulkan descriptor set layout.
     *
     * Defines the layout of the descriptor sets and creates a corresponding layout object.
     */
    void createDescriptorSetLayout();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the Vulkan descriptor pool.
     *
     * Allocates a descriptor pool from which descriptor sets can be allocated.
     */
    void createDescriptorPool();
};
// ================================================================================
// ================================================================================ 

/**
 * @class GraphicsPipeline
 * @brief Manages the Vulkan graphics pipeline and associated resources.
 *
 * This class encapsulates the creation and management of the Vulkan graphics pipeline,
 * including framebuffers, render passes, and shader modules. It also handles command buffer
 * recording for rendering.
 */
class GraphicsPipeline {
public:

    /**
     * @brief Constructor for GraphicsPipeline.
     *
     * Initializes the graphics pipeline by setting up the render pass, creating the pipeline layout, 
     * and building the graphics pipeline.
     *
     * @param device The Vulkan logical device handle.
     * @param swapChain Reference to the swap chain.
     * @param commandBufferManager Reference to the CommandBufferManager, used for managing command buffers.
     * @param bufferManager Reference to the BufferManager, which provides vertex and index buffers.
     * @param descriptorManager Reference to the DescriptorManager, which provides descriptor sets and layouts.
     * @param indices The index data for rendering.
     * @param physicalDevice The Vulkan physical device handle.
     */
    GraphicsPipeline(VkDevice device,
                     SwapChain& swapChain,
                     CommandBufferManager& commandBufferManager,
                     BufferManager& bufferManager,
                     DescriptorManager& descirptorManager,
                     const std::vector<uint16_t>& indices,
                     VkPhysicalDevice physicalDevice);
 // --------------------------------------------------------------------------------

    /**
     * @brief Destructor for GraphicsPipeline.
     *
     * Cleans up all Vulkan resources associated with the graphics pipeline,
     * including framebuffers, pipeline layout, and the render pass.
     */
    ~GraphicsPipeline();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates framebuffers for each swap chain image view.
     *
     * @param swapChainImageViews The image views from the swap chain.
     * @param swapChainExtent The extent of the swap chain, i.e., its width and height.
     */
    void createFrameBuffers(const std::vector<VkImageView>& swapChainImageViews, 
                            VkExtent2D swapChainExtent);
// --------------------------------------------------------------------------------

    /**
     * @brief Destroys all the framebuffers.
     *
     * This method cleans up and destroys the Vulkan framebuffers created for the swap chain.
     */
    void destroyFramebuffers();
// --------------------------------------------------------------------------------

    /**
     * @brief Records command buffer for a specific frame and image index.
     *
     * This method records the commands needed to render a frame, including setting up the
     * render pass, binding the graphics pipeline, and drawing indexed vertex data.
     *
     * @param frameIndex The index of the current frame in flight.
     * @param imageIndex The index of the swap chain image being rendered to.
     */
    void recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the pipeline layout.
     *
     * @return A reference to the Vulkan pipeline layout.
     */
    const VkPipelineLayout& getPipelineLayout() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan graphics pipeline.
     *
     * @return A reference to the Vulkan graphics pipeline.
     */
    const VkPipeline& getPipeline() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan render pass.
     *
     * @return A reference to the Vulkan render pass.
     */
    const VkRenderPass& getRenderPass() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of Vulkan framebuffers.
     *
     * @return A reference to the vector of Vulkan framebuffers.
     */
    const std::vector<VkFramebuffer>& getFrameBuffers() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves a specific framebuffer by frame index.
     *
     * @param frameIndex The index of the framebuffer to retrieve.
     * @return A reference to the Vulkan framebuffer.
     */
    const VkFramebuffer& getFrameBuffer(uint32_t frameIndex) const;
// ================================================================================
private:
    VkDevice device;                          /**< Vulkan logical device handle. */
    SwapChain& swapChain;                     /**< Reference to the swap chain. */
    CommandBufferManager& commandBufferManager;/**< Reference to the command buffer manager. */
    BufferManager& bufferManager;             /**< Reference to the buffer manager. */
    DescriptorManager& descriptorManager;     /**< Reference to the descriptor manager. */
    std::vector<uint16_t> indices;            /**< Index data for rendering. */
    VkPhysicalDevice physicalDevice;          /**< Vulkan physical device handle. */

    VkPipelineLayout pipelineLayout;          /**< The Vulkan pipeline layout. */
    VkPipeline graphicsPipeline;              /**< The Vulkan graphics pipeline. */
    VkRenderPass renderPass;                  /**< The Vulkan render pass. */
    std::vector<VkFramebuffer> framebuffers;  /**< Framebuffers for each swap chain image. */
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a shader module from the provided SPIR-V bytecode.
     *
     * @param code The SPIR-V bytecode of the shader.
     * @return The created shader module.
     */
    VkShaderModule createShaderModule(const std::vector<char>& code);
// --------------------------------------------------------------------------------

    /**
     * @brief Reads a file into a vector of characters.
     *
     * @param filename The path to the file.
     * @return The file content as a vector of characters.
     */
    std::vector<char> readFile(const std::string& filename);
// --------------------------------------------------------------------------------

    /**
     * @brief Finds a suitable memory type for Vulkan memory allocations.
     *
     * @param typeFilter A bitmask specifying the memory types to consider.
     * @param properties Required memory properties (e.g., device-local or host-visible).
     * @return The index of a suitable memory type.
     */
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the Vulkan render pass.
     *
     * @param swapChainImageFormat The format of the swap chain images.
     */
    void createRenderPass(VkFormat swapChainImageFormat);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the graphics pipeline.
     *
     * Sets up all the pipeline stages, including shaders, input assembly, and rasterization.
     */
    void createGraphicsPipeline();
};
// ================================================================================
// ================================================================================
#endif /* graphics_HPP */
// ================================================================================
// ================================================================================
// eof
