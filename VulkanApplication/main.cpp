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

// Function to generate circle vertices and indices
void generateCircleData(float radius, int segmentCount, std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) {
    vertices.clear();
    indices.clear();

    // Center vertex
    vertices.push_back({{0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}); // Center vertex with white color

    // Edge vertices
    for (int i = 0; i < segmentCount; ++i) {
        float angle = 2.0f * M_PI * i / segmentCount;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.push_back({{x, y}, {1.0f, 0.0f, 0.0f}}); // Edge vertices with red color
    }

    // Indices for triangle fan
    for (int i = 1; i <= segmentCount; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back((i % segmentCount) + 1);
    }
}

// Begin code
int main(int argc, const char * argv[]) {
    // Define vertices for two connected triangles  
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    }; 
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    }; 
    // Generate vertices for a circle
    // const float radius = 0.5f; // Radius of the circle
    // const int segmentCount = 36; // Number of segments to approximate the circle
    //
    // std::vector<Vertex> vertices;
    // std::vector<uint16_t> indices;
    //
    // generateCircleData(radius, segmentCount, vertices, indices);
    // Call Application 
    try {
        std::unique_ptr<Window> window = std::make_unique<GlfwWindow>(750, 900, "Vulkan Application", false);
        
        VulkanApplication triangle(std::move(window), vertices, indices);

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

