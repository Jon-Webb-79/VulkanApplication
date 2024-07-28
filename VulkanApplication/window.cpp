// ================================================================================
// ================================================================================
// - File:    window.cpp
// - Purpose: Contains implementation for window.hpp file
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 19, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/window.hpp"

#include <stdexcept>
#include <iostream>
// ================================================================================
// ================================================================================


GlfwWindow::GlfwWindow(uint32_t h, 
                       uint32_t w, 
                       const std::string& screen_title, 
                       bool full_screen) {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW Initialization Failed!\n");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* monitor = full_screen ? glfwGetPrimaryMonitor() : nullptr;

    width = w;
    height = h;
    window = glfwCreateWindow(w, h, screen_title.c_str(), monitor, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("GLFW Instantiation failed!\n");
    }
}
// --------------------------------------------------------------------------------

GlfwWindow::~GlfwWindow() {
    if (window)
        glfwDestroyWindow(window);
    if (glfw_instance) {
        glfwTerminate();
        glfw_instance = false;
    }
}
// --------------------------------------------------------------------------------

bool GlfwWindow::windowShouldClose() {
    return glfwWindowShouldClose(window);
}
// --------------------------------------------------------------------------------

void GlfwWindow::pollEvents() {
    glfwPollEvents();
}
// --------------------------------------------------------------------------------

bool GlfwWindow::isInstance() {
    return glfw_instance;
}
// --------------------------------------------------------------------------------

const char** GlfwWindow::getRequiredInstanceExtensions(uint32_t* count) {
    return glfwGetRequiredInstanceExtensions(count);
}
// --------------------------------------------------------------------------------


VkResult GlfwWindow::createWindowSurface(VkInstance instance,
                                         const VkAllocationCallbacks* allocator,
                                         VkSurfaceKHR* surface) {
    return glfwCreateWindowSurface(instance, window, allocator, surface);
}
// --------------------------------------------------------------------------------

void GlfwWindow::getFrameBufferSize() {
    glfwGetFramebufferSize(window, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
}
// --------------------------------------------------------------------------------

uint32_t GlfwWindow::getWidth() {
    return width;
}
// --------------------------------------------------------------------------------

uint32_t GlfwWindow::getHeight() {
    return height;
}
// ================================================================================
// ================================================================================
// eof
