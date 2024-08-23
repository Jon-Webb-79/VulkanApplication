// ================================================================================
// ================================================================================
// - File:    window.hpp
// - Purpose: This file contains various methods for implementing a window manager 
//            to support a Vulkan application
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 19, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include headers here

#ifndef window_HPP
#define window_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
// ================================================================================
// ================================================================================ 


class GlfwWindow {
public:
  
    /**
     * @brief Constructs a new GlfwWindow instance.
     * 
     * Initializes GLFW, creates a window, and sets it up with the specified dimensions,
     * title, and fullscreen mode.
     * 
     * @param h The height of the window in pixels.
     * @param w The width of the window in pixels.
     * @param screen_title The title of the window.
     * @param full_screen Whether the window should be fullscreen. Default is false.
     */ 
    GlfwWindow(uint32_t h, uint32_t w, const std::string& screen_title, bool full_screen = false);
// --------------------------------------------------------------------------------

    /**
     * @brief Destroys the GlfwWindow instance.
     * 
     * Destroys the GLFW window and terminates GLFW if it was not already terminated.
     */
    ~GlfwWindow();
// --------------------------------------------------------------------------------

    /**
     * @brief Checks if the GLFW window should close.
     * 
     * @return true if the window should close, false otherwise.
     */
    bool windowShouldClose();
// --------------------------------------------------------------------------------
    
    /**
     * @brief Polls events for the GLFW window.
     */
    void pollEvents();
// --------------------------------------------------------------------------------

    /**
     * @brief Checks if GLFW has been terminated.
     * 
     * @return true if GLFW has been terminated, false otherwise.
     */
    bool isInstance();
// --------------------------------------------------------------------------------

    /**
    * @brief Retrieve the required instance extensions for GLFW.
    * 
    * This method calls glfwGetRequiredInstanceExtensions to get the instance extensions
    * required by GLFW for Vulkan to interface with the windowing system.
    * 
    * @param[out] count Pointer to an unsigned integer where the number of extensions will be stored.
    * @return const char** An array of strings containing the names of the required extensions.
    */
    const char** getRequiredInstanceExtensions(uint32_t* count);
// --------------------------------------------------------------------------------

    /**
     * @brief Method to create a window surface for interaction with Vulkan 
     *
     * This pure virtual method is used to establish an interface between the 
     * glfwWindow window and a Vulkan instance.
     *
     * @param instance A Vulkan instance 
     * @param allocator The required allocator or nullptr
     * @param surface Where to store the handle of the surface.  This is set to 
     *                VK_NULL_HANDLE if an error occurred
     * @return VkResult 
     */
    VkResult createWindowSurface(VkInstance instance, 
                                 const VkAllocationCallbacks* allocator,
                                 VkSurfaceKHR* surface);
// --------------------------------------------------------------------------------

    /**
     * @brief This method retrives the size, in pixels, of hte framebuffer of the specified window
     */
    void getFrameBufferSize();
// --------------------------------------------------------------------------------

    /**
     * @brief returns the width of a screen in units of pixesl
     */
    uint32_t getWidth();
// --------------------------------------------------------------------------------

    /**
     * @brief Returns the height of a screen in units of pixels
     */
    uint32_t getHeight();
// --------------------------------------------------------------------------------

    void setFramebufferResizeCallback(void (*callback)(GLFWwindow*, int, int));
// --------------------------------------------------------------------------------

    GLFWwindow* getGLFWWindow() const;
// ================================================================================
private:

    uint32_t height;
    uint32_t width;
    GLFWwindow* window;
// --------------------------------------------------------------------------------

    /**
     * @brief Indicates whether GLFW has been terminated.
     */
    bool glfw_instance = true;
};
// ================================================================================ 
// ================================================================================
#endif /* file_name_HPP */
// ================================================================================
// ================================================================================
// eof
