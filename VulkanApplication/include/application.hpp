// ================================================================================
// ================================================================================
// - File:    application.hpp
// - Purpose: This file contains a basic application interface for building and 
//            displaying a triangle to the screen.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 19, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef application_HPP
#define application_HPP

#define GLFW_INCLUDE_VULKAN  // <vulkan/vulkan.h>
#include "window.hpp"
#include "validation_layers.hpp"
#include "graphics_pipeline.hpp"
#include "devices.hpp"

#include <memory>
// ================================================================================
// ================================================================================

/**
 * @brief This class creates an instance of Vulkan to support an 
 * application that will draw a triangle to the screen
 */
class VulkanInstance {
public:
    /**
     * @brief Constructor for the VulkanInstance class 
     *
     * @param window A reference to a Window object
     */
    VulkanInstance(Window& window, ValidationLayers& validationLayers);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor for the VulkanInstance class
     */
    ~VulkanInstance();
// --------------------------------------------------------------------------------

    /**
     * @brief Returns a raw pointer to the instance of Vulkan
     */
    VkInstance* getInstance();
// --------------------------------------------------------------------------------

    /**
     * @brief Returns a raw pointer to an instance of surface
     */
    VkSurfaceKHR getSurface() const;
// ================================================================================
private:
    Window& windowInstance;
    ValidationLayers& validationLayers;
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
// --------------------------------------------------------------------------------

    /**
     * @brief Helper function that allows the constructor to create a Vulkan instance
     */
    void createInstance();
// --------------------------------------------------------------------------------

    /**
     * @brief Helper function that establishes a relationship between Vulkan and the window
     */
    void createSurface();
};
// ================================================================================ 
// ================================================================================


class VulkanApplication {
public:
    /**
     * @brief Constructs a new VulkanApplication instance.
     * 
     * @param window A reference to a Window object that the application will use.
     * @param vertices A vector of Vertex objects
     */
    VulkanApplication(std::unique_ptr<Window> window, 
                      const std::vector<Vertex>& vertices,
                      const std::vector<uint16_t>& indices);
// --------------------------------------------------------------------------------

    /**
     * @brief Releases all dynamically allocated memory for application
     */
    ~VulkanApplication();
// --------------------------------------------------------------------------------

    /**
     * @brief Runs the main application loop
     *
     * This method starts the main rendering loop of the application, polling events
     * and rendering frames until the window is closed
     */
    void run();
// ================================================================================
private:

    std::unique_ptr<Window> windowInstance;
    std::unique_ptr<ValidationLayers> validationLayers;
    std::unique_ptr<VulkanInstance> vulkanInstanceCreator;
    std::unique_ptr<VulkanPhysicalDevice> vulkanPhysicalDevice; 
    std::unique_ptr<VulkanLogicalDevice> vulkanLogicalDevice;
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    VkQueue graphicsQueue; // = VK_NULL_HANDLE;
    VkQueue presentQueue; // = VK_NULL_HANDLE;

    uint32_t currentFrame = 0;
// --------------------------------------------------------------------------------

    /**
     * @brief This method is used by the destructor to free all memory in a specific
     * order.
     */
    void destroyResources();
// --------------------------------------------------------------------------------

    /**
    * @brief Draws a frame by acquiring an image from the swap chain, recording a command buffer, and submitting it to the graphics queue.
     *
    * This method handles the process of drawing a frame in the Vulkan application. It performs the following steps:
    * 1. Waits for the previous frame to finish using synchronization objects.
    * 2. Acquires an image from the swap chain to render to.
    * 3. Resets the command buffer and records commands to it.
    * 4. Submits the recorded command buffer to the graphics queue for execution.
    * 5. Presents the rendered image to the swap chain.
    *
    * @throws std::runtime_error if any Vulkan function fails.
    */
    void drawFrame();
};
// ================================================================================
// ================================================================================
#endif /* application_HPP */
// eof
