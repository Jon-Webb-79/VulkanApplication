// ================================================================================
// ================================================================================
// - File:    devices.hpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 22, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef devices_HPP
#define devices_HPP

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "queues.hpp"
#include <memory>
#include <vector>
#include <mutex>
// ================================================================================
// ================================================================================ 

/**
 * @class VulkanPhysicalDevice
 * @brief Represents a physical device in a Vulkan application.
 * 
 * This class is responsible for selecting a suitable physical device (GPU) from the available devices
 * that support Vulkan. It checks if the device is suitable based on specific criteria.
 */
class VulkanPhysicalDevice {
public:
    /**
     * @brief Constructs a VulkanPhysicalDevice object.
     * 
     * This constructor initializes the VulkanPhysicalDevice by selecting a suitable physical device
     * from the available devices that support Vulkan.
     * 
     * @param instance A reference to the Vulkan instance.
     */
    VulkanPhysicalDevice(VkInstance& instance, VkSurfaceKHR surface);
// --------------------------------------------------------------------------------

    /**
     * @brief Move constructor for VulkanPhysicalDevice.
     * 
     * This constructor transfers ownership of the resources managed by the given
     * VulkanPhysicalDevice object (`other`) to the new object being created.
     * After the move, the source object (`other`) is left in a valid but unspecified
     * state. This ensures efficient transfer of resources without copying.
     * 
     * @param other The VulkanPhysicalDevice object to be moved. After the move, 
     * `other` will be left in a valid but unspecified state with its resources
     * transferred to the new object.
     */
    VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept;
// --------------------------------------------------------------------------------

    /**
     * @brief Move assignment operator for VulkanPhysicalDevice.
     * 
     * This operator transfers ownership of the resources from the given
     * VulkanPhysicalDevice object (`other`) to the current object (`*this`).
     * It releases any resources currently held by the current object before
     * taking ownership of the resources from `other`. After the move, the source
     * object (`other`) is left in a valid but unspecified state.
     * 
     * @param other The VulkanPhysicalDevice object to be moved. After the move, 
     * `other` will be left in a valid but unspecified state with its resources
     * transferred to the current object.
     * 
     * @return A reference to the current VulkanPhysicalDevice object (`*this`).
     */
    VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&& other) noexcept;
// --------------------------------------------------------------------------------

    /**
     * @brief Deleted copy constructor.
     * 
     * This constructor is deleted to prevent accidental copying of the VulkanPhysicalDevice
     * object. Copying Vulkan resources is usually undesirable because it can lead to
     * resource management issues such as double-free errors or unintended resource sharing.
     * 
     * @param other The VulkanPhysicalDevice object to be copied (deleted operation).
     */
    VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
// --------------------------------------------------------------------------------

    /**
     * @brief Deleted copy assignment operator.
     * 
     * This operator is deleted to prevent accidental copying of the VulkanPhysicalDevice
     * object. Copying Vulkan resources is usually undesirable because it can lead to
     * resource management issues such as double-free errors or unintended resource sharing.
     * 
     * @param other The VulkanPhysicalDevice object to be copied (deleted operation).
     * 
     * @return A reference to the current VulkanPhysicalDevice object (deleted operation).
     */
    VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the selected physical device.
     * 
     * @return The Vulkan physical device handle.
     */
    const VkPhysicalDevice getDevice() const;
// ================================================================================

private:
    VkInstance& instance;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    mutable std::mutex deviceMutex;
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
// --------------------------------------------------------------------------------
    
    /**
     * @brief Checks if a physical device is suitable for the application.
     * 
     * @param device The Vulkan physical device to check.
     * @return True if the device is suitable, false otherwise.
     */
    bool isDeviceSuitable(const VkPhysicalDevice device) const;
// --------------------------------------------------------------------------------
    /**
    * @brief Checks if the specified physical device supports all required device extensions.
    *
    * This method queries the specified Vulkan physical device to determine if it supports the required
    * device extensions specified in the application. It does this by enumerating the available device
    * extension properties and checking if all the required extensions are present.
    *
    * @param device The Vulkan physical device to check for extension support.
    * @return true if all required device extensions are supported, false otherwise.
    *
    * @throws std::runtime_error if the extension properties cannot be enumerated.
    */
    bool checkDeviceExtensionSupport(const VkPhysicalDevice& device) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Rates the suitability of a given Vulkan physical device for the application.
     * 
     * This function assigns a score to a Vulkan physical device based on its properties and 
     * features, such as GPU type, maximum texture size, and support for specific Vulkan features.
     * The scoring system prioritizes discrete GPUs over integrated ones, and gives higher scores 
     * to devices with better performance characteristics. The device with the highest score 
     * is considered the most suitable for the application.
     * 
     * @param device The Vulkan physical device to be evaluated.
     * 
     * @return An integer score representing the suitability of the device. Higher scores 
     * indicate better suitability. If the device lacks critical features required by the 
     * application (e.g., geometry shaders), the function returns a score of zero, making the 
     * device ineligible for selection.
     */
    int rateDeviceSuitability(const VkPhysicalDevice device);
};
// ================================================================================
// ================================================================================

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
// ================================================================================
// ================================================================================

/**
 * @brief Manages the Vulkan swap chain, including creation, image views, and cleanup.
 *
 * The SwapChain class is responsible for creating and managing the Vulkan swap chain, which includes
 * handling the swap chain images and their associated image views. It also provides utility methods
 * for querying swap chain support details and choosing optimal swap chain settings.
 */
class SwapChain {
public:
    /**
     * @brief Constructs a SwapChain object and initializes the swap chain.
     *
     * @param device The Vulkan logical device.
     * @param surface The Vulkan surface.
     * @param physicalDevice The Vulkan physical device.
     * @param window A pointer to the Window object.
     *
     * @throws std::runtime_error if the swap chain or image views cannot be created.
     */
    SwapChain(VkDevice device, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, GLFWwindow* window);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor for the SwapChain object.
     *
     * Cleans up the swap chain and its associated image views.
     */
    ~SwapChain();
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan swap chain.
     *
     * @return The Vulkan swap chain (VkSwapchainKHR).
     */
    VkSwapchainKHR getSwapChain() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the format of the swap chain images.
     *
     * @return The format of the swap chain images (VkFormat).
     */
    VkFormat getSwapChainImageFormat() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the extent (dimensions) of the swap chain images.
     *
     * @return The extent (dimensions) of the swap chain images (VkExtent2D).
     */
    VkExtent2D getSwapChainExtent() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the swap chain images.
     *
     * @return A reference to a vector containing the swap chain images.
     */
    const std::vector<VkImage>& getSwapChainImages() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the swap chain image views.
     *
     * @return A reference to a vector containing the swap chain image views.
     */
    const std::vector<VkImageView>& getSwapChainImageViews() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Queries the swap chain support details for a physical device and surface.
     *
     * @param device The Vulkan physical device.
     * @param surface The Vulkan surface.
     * @return The swap chain support details (SwapChainSupportDetails).
     */
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
// --------------------------------------------------------------------------------


    void cleanupSwapChain();
// --------------------------------------------------------------------------------

    void recreateSwapChain();
// ================================================================================
private:
    VkDevice device;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    GLFWwindow* window;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the swap chain.
     *
     * @throws std::runtime_error if the swap chain cannot be created.
     */
    void createSwapChain();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the image views for the swap chain images.
     *
     * @throws std::runtime_error if the image views cannot be created.
     */
    void createImageViews();
// --------------------------------------------------------------------------------

    /**
     * @brief Cleans up the image views.
     */
    void cleanupImageViews();
// --------------------------------------------------------------------------------

    /**
     * @brief Chooses the best surface format for the swap chain from the available formats.
     *
     * @param availableFormats The list of available surface formats.
     * @return The chosen surface format (VkSurfaceFormatKHR).
     */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
// --------------------------------------------------------------------------------

    /**
     * @brief Chooses the best present mode for the swap chain from the available present modes.
     *
     * @param availablePresentModes The list of available present modes.
     * @return The chosen present mode (VkPresentModeKHR).
     */
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
// --------------------------------------------------------------------------------

    /**
     * @brief Chooses the best extent (dimensions) for the swap chain images based on the capabilities.
     *
     * @param capabilities The surface capabilities.
     * @return The chosen extent (VkExtent2D).
     */
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};
// ================================================================================
// ================================================================================

/**
 * @class VulkanLogicalDevice
 * @brief Represents a logical device in a Vulkan application.
 * 
 * This class is responsible for creating and managing a logical device (VkDevice) and its associated
 * graphics queue. It requires a physical device and optionally validation layers.
 */
class VulkanLogicalDevice {
public:
    
    /**
     * @brief Constructs a VulkanLogicalDevice object.
     * 
     * This constructor initializes the VulkanLogicalDevice by creating a logical device and its
     * associated graphics queue.
     * 
     * @param physicalDevice A reference to the Vulkan physical device.
     * @param validationLayers A vector containing the names of the validation layers to be enabled.
     * @param surface A VkSurfaceKHR data type
     */
    VulkanLogicalDevice(VkPhysicalDevice physicalDevice, 
                        const std::vector<const char*>& validationLayers,
                        VkSurfaceKHR surface,
                        const std::vector<const char*>& deviceExtensions);
// --------------------------------------------------------------------------------
    
    /**
     * @brief Destroys the VulkanLogicalDevice object.
     * 
     * This destructor cleans up the logical device by destroying it if it has been created.
     */
    ~VulkanLogicalDevice();
// --------------------------------------------------------------------------------
    
    /**
     * @brief Retrieves the logical device.
     * 
     * @return The Vulkan logical device handle.
     */
    VkDevice getDevice() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the graphics queue.
     * 
     * @return The Vulkan graphics queue handle.
     */
    VkQueue getGraphicsQueue() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrives the present queue 
     *
     * @return The vulkan queue handle
     */
    VkQueue getPresentQueue() const;
// ================================================================================
private:
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkPhysicalDevice physicalDevice;  // Changed to reference
    const std::vector<const char*>& validationLayers;
    VkSurfaceKHR surface;
    const std::vector<const char*>& deviceExtensions; 
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the logical device and retrieves the graphics queue.
     */
    void createLogicalDevice();
};
// ================================================================================
// ================================================================================ 
#endif /* devices_HPP */
