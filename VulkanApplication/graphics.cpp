// ================================================================================
// ================================================================================
// - File:    graphics.cpp
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

#include "include/graphics.hpp"
#include "include/queues.hpp"
#include <iostream>

#include <cstring>  // memcpy
#include <string>
#include <fstream>
#include <filesystem>
// ================================================================================
// ================================================================================


CommandBufferManager::CommandBufferManager(VkDevice device,
                                           const std::vector<uint16_t>& indices,
                                           VkPhysicalDevice physicalDevice,
                                           VkSurfaceKHR surface)
    : device(device),
      indices(indices) {
    // Create initial size for vectors
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT),
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT),
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    // Instantiate attributes
    createCommandPool(physicalDevice, surface);
    createSyncObjects();
    createCommandBuffers();
}
// --------------------------------------------------------------------------------

CommandBufferManager::~CommandBufferManager() {
    if (device != VK_NULL_HANDLE) {
        // Free command buffers before destroying the command pool
        if (!commandBuffers.empty() && commandPool != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (imageAvailableSemaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            }
            if (renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            }
            if (inFlightFences[i] != VK_NULL_HANDLE) {
                vkDestroyFence(device, inFlightFences[i], nullptr);
            }
        }

        if (commandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device, commandPool, nullptr);
        }
    }
}
// --------------------------------------------------------------------------------

void CommandBufferManager::waitForFences(uint32_t frameIndex) const {
    VkResult result = vkWaitForFences(device, 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
    std::string msg = std::string("Failed to wait for fence at frame index ") + 
                      std::to_string(frameIndex) + 
                      ". Error code: " + 
                      std::to_string(static_cast<int>(result));

    if (result != VK_SUCCESS) {
        throw std::runtime_error(msg);
    }
}
// --------------------------------------------------------------------------------

void CommandBufferManager::resetFences(uint32_t frameIndex) const {
    VkResult result = vkResetFences(device, 1, &inFlightFences[frameIndex]);
    std::string msg = std::string("Failed to reset fence at frame index ") + 
                      std::to_string(frameIndex) + 
                      ". Error code: " + 
                      std::to_string(result);
    if (result != VK_SUCCESS) {
       throw std::runtime_error(msg); 
    }
}
// --------------------------------------------------------------------------------

const VkCommandPool& CommandBufferManager::getCommandPool() const {
    if (commandPool == VK_NULL_HANDLE) {
        throw std::runtime_error("Command pool is not initialized.");
    }
    return commandPool;
}
// --------------------------------------------------------------------------------

const std::vector<VkCommandBuffer>& CommandBufferManager::getCommandBuffers() const {
    if (commandBuffers.empty()) {
        throw std::runtime_error("Command buffers are not allocated.");
    }
    return commandBuffers;
}
// --------------------------------------------------------------------------------

const VkCommandBuffer& CommandBufferManager::getCommandBuffer(uint32_t frameIndex) const {
    if (commandBuffers[frameIndex] == VK_NULL_HANDLE) {
        throw std::runtime_error(std::string("Command Buffer .") +
                                 std::to_string(frameIndex) +
                                 std::string(" does not exist!"));
    }
    return commandBuffers[frameIndex];
}
// --------------------------------------------------------------------------------

const VkSemaphore& CommandBufferManager::getImageAvailableSemaphore(uint32_t frameIndex) const {
    if (imageAvailableSemaphores[frameIndex] == VK_NULL_HANDLE)
        throw std::runtime_error(std::string("Image available semaphore ") +
                                             std::to_string(frameIndex) +
                                             std::string(" does not exist!"));
    return imageAvailableSemaphores[frameIndex];
}
// --------------------------------------------------------------------------------

const VkSemaphore& CommandBufferManager::getRenderFinishedSemaphore(uint32_t frameIndex) const {
    if (renderFinishedSemaphores[frameIndex] == VK_NULL_HANDLE)
        throw std::runtime_error(std::string("Render Finished Semaphore ") +
                                 std::to_string(frameIndex) +
                                 std::string(" does not exist!"));
    return renderFinishedSemaphores[frameIndex];
}
// --------------------------------------------------------------------------------

const VkFence& CommandBufferManager::getInFlightFence(uint32_t frameIndex) const {
    if (inFlightFences[frameIndex] == VK_NULL_HANDLE)
        throw std::runtime_error(std::string("In Flight Fence ") +
                                 std::to_string(frameIndex) +
                                 std::string(" does not exist!"));
    return inFlightFences[frameIndex];
}
// ================================================================================

void CommandBufferManager::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    auto createSemaphore = [this]() -> VkSemaphore {
        VkSemaphore semaphore;
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphore!");
        }
        return semaphore;
    };

    auto createFence = [this]() -> VkFence {
        VkFence fence;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create fence!");
        }
        return fence;
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores[i] = createSemaphore();
        renderFinishedSemaphores[i] = createSemaphore();
        inFlightFences[i] = createFence();
    }
}
// --------------------------------------------------------------------------------

void CommandBufferManager::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
    std::string msg = std::string("Failed to create command pool!: Error code: ") + 
                      std::to_string((result));
    if (result != VK_SUCCESS) {
        throw std::runtime_error(msg);
    }  
}
// --------------------------------------------------------------------------------

void CommandBufferManager::createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices queueFamilyIndices = QueueFamily::findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    std::string msg = std::string("Failed to create command pool!: Error code: ") + 
                      std::to_string((result));
    if (result != VK_SUCCESS) {
        throw std::runtime_error(msg);
    }
}
// ================================================================================
// ================================================================================


BufferManager::BufferManager(const std::vector<Vertex>& vertices,
                             const std::vector<uint16_t>& indices,
                             AllocatorManager& allocatorManager,
                             CommandBufferManager& commandBufferManager,
                             VkQueue graphicsQueue)
    : vertices(vertices),
      indices(indices),
      allocatorManager(allocatorManager),
      commandBufferManager(commandBufferManager),
     // commandPool(commandPool),
      graphicsQueue(graphicsQueue){
    // Set initial vector sizes
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
}
// --------------------------------------------------------------------------------

BufferManager::~BufferManager() {
    // Clean up uniform buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (uniformBuffers[i] != VK_NULL_HANDLE) {
            // Unmap the memory if it was mapped
            if (uniformBuffersMapped[i] != nullptr) {
                vmaUnmapMemory(allocatorManager.getAllocator(), uniformBuffersMemory[i]);
            }
            // Destroy the buffer and free the associated memory
            allocatorManager.destroyBuffer(uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    // Clean up vertex buffer
    if (vertexBuffer != VK_NULL_HANDLE) {
        allocatorManager.destroyBuffer(vertexBuffer, vertexBufferAllocation);
    }

    // Clean up index buffer
    if (indexBuffer != VK_NULL_HANDLE) {
        allocatorManager.destroyBuffer(indexBuffer, indexBufferAllocation);
    }
}
// --------------------------------------------------------------------------------

void BufferManager::updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo) {
    // Ensure that the current frame index is within bounds
    if (currentFrame >= MAX_FRAMES_IN_FLIGHT) {
        throw std::out_of_range("Frame index out of bounds.");
    }

    // Map the memory for the current frame's uniform buffer
    void* data = uniformBuffersMapped[currentFrame];
    if (data == nullptr) {
        throw std::runtime_error(std::string("Failed to map uniform buffer for frame ") + std::to_string(currentFrame));
    }

    // Copy the UniformBufferObject data into the mapped memory
    memcpy(data, &ubo, sizeof(ubo));
}

// --------------------------------------------------------------------------------

const VkBuffer BufferManager::getVertexBuffer() const{
    return vertexBuffer;
}
// --------------------------------------------------------------------------------

const VkBuffer BufferManager::getIndexBuffer() const {
    return indexBuffer;
}
// --------------------------------------------------------------------------------

const std::vector<VkBuffer>& BufferManager::getUniformBuffers() const {
    return uniformBuffers;
}
// --------------------------------------------------------------------------------

const std::vector<void*>& BufferManager::getUniformBuffersMapped() const {
    return uniformBuffersMapped;
}
// ================================================================================

bool BufferManager::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Step 1: Create a staging buffer
    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    try {
        allocatorManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                      VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer, stagingBufferAllocation);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }

    // Step 2: Map memory and copy vertex data to the staging buffer
    void* data;
    try {
        allocatorManager.mapMemory(stagingBufferAllocation, &data);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation); // Cleanup
        return false;
    }
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    allocatorManager.unmapMemory(stagingBufferAllocation);

    // Step 3: Create the vertex buffer on the GPU
    try {
        allocatorManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                                      VMA_MEMORY_USAGE_GPU_ONLY, vertexBuffer, vertexBufferAllocation);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation); // Cleanup
        return false;
    }

    // Step 4: Copy data from the staging buffer to the vertex buffer
    try {
        allocatorManager.copyBuffer(stagingBuffer, vertexBuffer, bufferSize, graphicsQueue, commandBufferManager.getCommandPool());
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation); // Cleanup
        allocatorManager.destroyBuffer(vertexBuffer, vertexBufferAllocation);   // Cleanup
        return false;
    }

    // Step 5: Clean up the staging buffer
    allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation);

    return true; // Indicate success
}
// --------------------------------------------------------------------------------

bool BufferManager::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Step 1: Create a staging buffer
    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    try {
        allocatorManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                      VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer, stagingBufferAllocation);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }

    // Step 2: Map memory and copy index data to the staging buffer
    void* data;
    try {
        allocatorManager.mapMemory(stagingBufferAllocation, &data);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation); // Cleanup
        return false;
    }
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    allocatorManager.unmapMemory(stagingBufferAllocation);

    // Step 3: Create the index buffer on the GPU
    try {
        allocatorManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                                      VMA_MEMORY_USAGE_GPU_ONLY, indexBuffer, indexBufferAllocation);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation); // Cleanup
        return false;
    }

    // Step 4: Copy data from the staging buffer to the index buffer
    try {
        allocatorManager.copyBuffer(stagingBuffer, indexBuffer, bufferSize, graphicsQueue, commandBufferManager.getCommandPool());
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation); // Cleanup
        allocatorManager.destroyBuffer(indexBuffer, indexBufferAllocation);     // Cleanup
        return false;
    }

    // Step 5: Clean up the staging buffer
    allocatorManager.destroyBuffer(stagingBuffer, stagingBufferAllocation);

    return true; // Indicate success
}
// --------------------------------------------------------------------------------

bool BufferManager::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    // Resize the buffers to the correct size for the number of frames in flight
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        try {
            // Step 1: Create a uniform buffer for each frame
            allocatorManager.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                                          VMA_MEMORY_USAGE_CPU_TO_GPU, 
                                          uniformBuffers[i], uniformBuffersMemory[i]);
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            // Cleanup for previously created buffers
            for (size_t j = 0; j < i; j++) {
                allocatorManager.destroyBuffer(uniformBuffers[j], uniformBuffersMemory[j]);
            }
            return false;
        }

        try {
            // Step 2: Map the uniform buffer memory
            allocatorManager.mapMemory(uniformBuffersMemory[i], &uniformBuffersMapped[i]);
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            // Cleanup for previously created buffers
            for (size_t j = 0; j <= i; j++) {
                allocatorManager.destroyBuffer(uniformBuffers[j], uniformBuffersMemory[j]);
            }
            return false;
        }
    }

    return true; // Indicate success
}
// ================================================================================
// ================================================================================


DescriptorManager::DescriptorManager(VkDevice device)
    : device(device){
    createDescriptorSetLayout();
    createDescriptorPool();
}
// --------------------------------------------------------------------------------

    DescriptorManager::~DescriptorManager() {
    // Check and clean up descriptor set layout
    if (descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        descriptorSetLayout = VK_NULL_HANDLE;  // Reset to null handle after destruction
    }

    // Check and clean up descriptor pool (this will automatically free descriptor sets)
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;  // Reset to null handle after destruction
    }

    // Clear the descriptor sets vector to free any associated memory
    descriptorSets.clear();
}
// --------------------------------------------------------------------------------

void DescriptorManager::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)},
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}
// --------------------------------------------------------------------------------

void DescriptorManager::createDescriptorSets(const std::vector<VkBuffer> uniformBuffers) {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}
// --------------------------------------------------------------------------------

const VkDescriptorSetLayout& DescriptorManager::getDescriptorSetLayout() const{
    if (descriptorSetLayout == VK_NULL_HANDLE)
        throw std::runtime_error("Descriptor set layout is not initialized!");
    return descriptorSetLayout;
}
// --------------------------------------------------------------------------------

const VkDescriptorPool& DescriptorManager::getDescriptorPool() const {
    if (descriptorPool == VK_NULL_HANDLE)
        throw std::runtime_error("Descriptor pool is not initialized!");
    return descriptorPool;
}
// --------------------------------------------------------------------------------

const std::vector<VkDescriptorSet>& DescriptorManager::getDescriptorSets() const {
    if (descriptorSets.empty())
        throw std::runtime_error("Descriptor sets vector is empty!");
    return descriptorSets;
}
// --------------------------------------------------------------------------------

const VkDescriptorSet& DescriptorManager::getDescriptorSet(uint32_t frameIndex) const {
    // Bounds checking to prevent out-of-range access
    if (frameIndex >= descriptorSets.size()) {
        throw std::out_of_range("Frame index is out of bounds!");
    }
    
    if (descriptorSets[frameIndex] == VK_NULL_HANDLE) {
        throw std::runtime_error("Descriptor set for frame " + std::to_string(frameIndex) + " is not initialized!");
    }

    return descriptorSets[frameIndex];
}
// ================================================================================

void DescriptorManager::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    } 
}
// ================================================================================
// ================================================================================

GraphicsPipeline::GraphicsPipeline(VkDevice device,
                                   SwapChain& swapChain,
                                   CommandBufferManager& commandBufferManager,
                                   BufferManager& bufferManager,
                                   DescriptorManager& descriptorManager,  // Fixed typo here
                                   const std::vector<uint16_t>& indices,
                                   VkPhysicalDevice physicalDevice)
    : device(device),
      swapChain(swapChain),
      commandBufferManager(commandBufferManager),
      bufferManager(bufferManager),
      descriptorManager(descriptorManager),  // Correct initialization
      indices(indices),
      physicalDevice(physicalDevice) {
    createRenderPass(swapChain.getSwapChainImageFormat());
    createGraphicsPipeline();
}
// --------------------------------------------------------------------------------

GraphicsPipeline::~GraphicsPipeline() {
    // Clean up framebuffers
    for (auto framebuffer : framebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    }

    // Clean up pipeline-related resources
    if (graphicsPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
    }
    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }
}
// --------------------------------------------------------------------------------

void GraphicsPipeline::createFrameBuffers(const std::vector<VkImageView>& swapChainImageViews, 
                                          VkExtent2D swapChainExtent) {
    framebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
// --------------------------------------------------------------------------------

void GraphicsPipeline::destroyFramebuffers() {
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    framebuffers.clear();
}
// --------------------------------------------------------------------------------

void GraphicsPipeline::recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex) {
    VkCommandBuffer commandBuffer = commandBufferManager.getCommandBuffer(frameIndex);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error(std::string("failed to begin recording command buffer!") +
                                 std::to_string(frameIndex));
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffers[imageIndex];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChain.getSwapChainExtent().width;
    viewport.height = (float) swapChain.getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain.getSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { bufferManager.getVertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, bufferManager.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(
        commandBuffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        pipelineLayout, 
        0, 
        1, 
        &descriptorManager.getDescriptorSet(frameIndex), 
        0, 
        nullptr
    );

    //vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
// --------------------------------------------------------------------------------

const VkPipelineLayout& GraphicsPipeline::getPipelineLayout() const {
    if (pipelineLayout == VK_NULL_HANDLE)
        throw std::runtime_error("Graphics pipeline layout is not initialized!");
    return pipelineLayout;
}
// --------------------------------------------------------------------------------

const VkPipeline& GraphicsPipeline::getPipeline() const {
    if (graphicsPipeline == VK_NULL_HANDLE)
        throw std::runtime_error("Graphics pipeline is not initialized!");
    return graphicsPipeline;
}
// --------------------------------------------------------------------------------

const VkRenderPass& GraphicsPipeline::getRenderPass() const {
    if (renderPass == VK_NULL_HANDLE)
        throw std::runtime_error("Render pass is not initialized!");
    return renderPass;
}
// --------------------------------------------------------------------------------

const std::vector<VkFramebuffer>& GraphicsPipeline::getFrameBuffers() const {
    if (framebuffers.empty())
        std::runtime_error("Frame buffers vector is not populated!");
    return framebuffers;  
}
// --------------------------------------------------------------------------------

const VkFramebuffer& GraphicsPipeline::getFrameBuffer(uint32_t frameIndex) const {
    if (framebuffers.empty())
        std::runtime_error("Frame buffers vector is not populated!");
    if (frameIndex > size(framebuffers))
        std::runtime_error("Frame index is out of bounds!");
    return framebuffers[frameIndex];
}
// ================================================================================

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}
// --------------------------------------------------------------------------------

std::vector<char> GraphicsPipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Current working directory: " << std::filesystem::current_path() << std::endl;
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
// --------------------------------------------------------------------------------

uint32_t GraphicsPipeline::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
// --------------------------------------------------------------------------------

void GraphicsPipeline::createGraphicsPipeline() {
    auto vertShaderCode = readFile("../../shaders/shader.vert.spv");
    auto fragShaderCode = readFile("../../shaders/shader.frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;  // Set to 1 since you have one descriptor set layout
    pipelineLayoutInfo.pSetLayouts = &descriptorManager.getDescriptorSetLayout();  // Pass the descriptor set layout here
    pipelineLayoutInfo.pushConstantRangeCount = 0;  // No push constants for now

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}
// // --------------------------------------------------------------------------------
//
void GraphicsPipeline::createRenderPass(VkFormat swapChainImageFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
    }
}
// ================================================================================
// ================================================================================
// eof
