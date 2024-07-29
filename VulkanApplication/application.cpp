// ================================================================================
// ================================================================================
// - File:    application.cpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 19, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/application.hpp"
#include "include/constants.hpp"

#include <vector>
#include <iostream>
// ================================================================================
// ================================================================================

VulkanInstance::VulkanInstance(Window& window, ValidationLayers& validationLayers)
    : windowInstance(window), validationLayers(validationLayers) {

    createInstance();
    createSurface();
}
// --------------------------------------------------------------------------------


VulkanInstance::~VulkanInstance() {
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    if (instance != VK_NULL_HANDLE) {
        validationLayers.cleanup(instance);
        vkDestroyInstance(instance, nullptr);
    }
}
// --------------------------------------------------------------------------------


VkInstance* VulkanInstance::getInstance() {
    return &instance;
}
// --------------------------------------------------------------------------------

VkSurfaceKHR VulkanInstance::getSurface() const {
    return surface;
}
// ================================================================================


void VulkanInstance::createInstance() {
    if (validationLayers.isEnabled() && !validationLayers.checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
    // Populate VkApplicationInfo struct to describe this application
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanTriangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    // Variables used to help find required extensions
    uint32_t extensionCount = 0;

    const char** extensions = windowInstance.getRequiredInstanceExtensions(&extensionCount);

    std::vector<const char*> extensionVector(extensions, extensions + extensionCount);
   
    // If validation layers are enabled, add their required extensions
    if (validationLayers.isEnabled()) {
        std::vector<const char*> validationLayerExtensions = validationLayers.getRequiredExtensions();
        extensionVector.insert(extensionVector.end(), validationLayerExtensions.begin(), validationLayerExtensions.end());
    }
    // Implement VkInstanceCreateInfo struct
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionVector.size());
    createInfo.ppEnabledExtensionNames = extensionVector.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (validationLayers.isEnabled()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.getValidationLayers().size());
        createInfo.ppEnabledLayerNames = validationLayers.getValidationLayers().data(); 
        validationLayers.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
    }

    // Create the Vulkan instance
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to Create Vulkan Instance!");
    }

    if (validationLayers.isEnabled()) {
        validationLayers.setupDebugMessenger(instance);
    }
}
// --------------------------------------------------------------------------------

void VulkanInstance::createSurface() {
    if (windowInstance.createWindowSurface(instance, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface\n");
}
// ================================================================================
// ================================================================================


VulkanApplication::VulkanApplication(std::unique_ptr<Window> window, 
                                     const std::vector<Vertex>& vertices,
                                     const std::vector<uint16_t>& indices)
    : windowInstance(std::move(window)),
      vertices(vertices),
      indices(indices){
    // Instantiate related classes
    validationLayers = std::make_unique<ValidationLayers>(*this->windowInstance);
    vulkanInstanceCreator = std::make_unique<VulkanInstance>(*this->windowInstance, 
                                                             *validationLayers.get());
    vulkanPhysicalDevice = std::make_unique<VulkanPhysicalDevice>(*this->vulkanInstanceCreator->getInstance(),
                                                                  this->vulkanInstanceCreator->getSurface());
    vulkanLogicalDevice = std::make_unique<VulkanLogicalDevice>(vulkanPhysicalDevice->getPhysicalDevice(),
                                                                validationLayers->getValidationLayers(),
                                                                vulkanInstanceCreator->getSurface(),
                                                                deviceExtensions);
    swapChain = std::make_unique<SwapChain>(vulkanLogicalDevice->getDevice(),
                                            vulkanInstanceCreator->getSurface(),
                                            vulkanPhysicalDevice->getPhysicalDevice(),
                                            windowInstance.get());
    graphicsPipeline = std::make_unique<GraphicsPipeline>(vulkanLogicalDevice->getDevice(), 
                                                          swapChain->getSwapChainExtent(), 
                                                          swapChain->getSwapChainImageFormat(),
                                                          vulkanPhysicalDevice->getPhysicalDevice(),
                                                          vulkanLogicalDevice->getGraphicsQueue(),
                                                          vertices,
                                                          this->indices);
    graphicsPipeline->createFramebuffers(swapChain->getSwapChainImageViews(), 
                                     swapChain->getSwapChainExtent()); 
    graphicsPipeline->createCommandPool(vulkanPhysicalDevice->getPhysicalDevice(), 
                                    vulkanInstanceCreator->getSurface());
    graphicsPipeline->createVertexBuffer();
    graphicsPipeline->createIndexBuffer();
    graphicsPipeline->createCommandBuffers();
    graphicsPipeline->createSyncObjects();
    graphicsQueue = this->vulkanLogicalDevice->getGraphicsQueue();
    presentQueue = this->vulkanLogicalDevice->getPresentQueue();
}
// -------------------------------------------------------------------------------- 

VulkanApplication::~VulkanApplication() {
    destroyResources();
}
// --------------------------------------------------------------------------------

void VulkanApplication::run() {
    while (!windowInstance->windowShouldClose()) {
        windowInstance->pollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(vulkanLogicalDevice->getDevice());
}
// ================================================================================

void VulkanApplication::destroyResources() {

    graphicsPipeline.reset();
    swapChain.reset();

    // Destroy Vulkan logical device first
    vulkanLogicalDevice.reset();

    // Destroy other Vulkan resources
    vulkanPhysicalDevice.reset();
    vulkanInstanceCreator.reset();

    // Finally, destroy the window
    windowInstance.reset();
}
// --------------------------------------------------------------------------------

void VulkanApplication::drawFrame() {
    VkDevice device = vulkanLogicalDevice->getDevice();
    uint32_t frameIndex = currentFrame;
    
    // Wait for the frame to be finished
    graphicsPipeline->waitForFences(frameIndex);
    graphicsPipeline->resetFences(frameIndex);
    uint32_t imageIndex;
    if (vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, 
                              graphicsPipeline->getImageAvailableSemaphore(frameIndex), 
                              VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    VkCommandBuffer cmdBuffer = graphicsPipeline->getCommandBuffer(frameIndex);
    vkResetCommandBuffer(cmdBuffer, 0);
    graphicsPipeline->recordCommandBuffer(frameIndex, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {graphicsPipeline->getImageAvailableSemaphore(frameIndex)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkSemaphore signalSemaphores[] = {graphicsPipeline->getRenderFinishedSemaphore(frameIndex)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, graphicsPipeline->getInFlightFence(frameIndex)) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(presentQueue, &presentInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
// ================================================================================
// ================================================================================
// eof
