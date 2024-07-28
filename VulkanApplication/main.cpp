// ================================================================================
// ================================================================================
// - File:    main.cpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    July 26, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/application.hpp"
#include "include/window.hpp"

#include <iostream>
#include <stdexcept>
#include <memory>

// Begin code
int main(int argc, const char * argv[]) {
    // Define vertices 
    std::vector<Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    // Call Application 
    try {
        std::unique_ptr<Window> window = std::make_unique<GlfwWindow>(750, 900, "Vulkan Application", false);
        
        VulkanApplication triangle(std::move(window), vertices);

        triangle.run();
    } catch(const std::exception& e) {
        std::cerr << e.what() <<  "\n";
        return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}
// ================================================================================
// ================================================================================
// eof

