/*
Header-only STL file reader - UPDATED to handle both ASCII and BINARY formats
*/
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cstdint> // For uint32_t

namespace stl_reader {
    struct Point { float x, y, z; };
    struct Triangle { Point normal, v1, v2, v3; };

    // Helper function for binary reading
    inline Point read_point(std::ifstream& file) {
        Point p;
        file.read(reinterpret_cast<char*>(&p.x), sizeof(float));
        file.read(reinterpret_cast<char*>(&p.y), sizeof(float));
        file.read(reinterpret_cast<char*>(&p.z), sizeof(float));
        return p;
    }

    // Main read function
    inline std::vector<Triangle> read_stl(const std::string& filename) {
        std::vector<Triangle> triangles;
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open STL file: " + filename);
        }

        // --- NEW: Format Detection ---
        char buffer[6];
        file.read(buffer, 5);
        buffer[5] = '\0';
        std::string header_start(buffer);
        file.seekg(0); // Rewind to the beginning of the file

        if (header_start == "solid") {
            // --- ASCII STL Parsing ---
            std::cout << "  Detected ASCII STL format." << std::endl;
            file.close(); // <-- THIS IS THE FIX: Close the file before re-opening
            file.clear();
            file.open(filename); // Re-open in text mode
            if (!file) throw std::runtime_error("Could not re-open STL file for ASCII reading.");

            std::string line;
            std::getline(file, line); // Skip "solid ..."
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string token;
                ss >> token;
                if (token == "endsolid") break;
                if (token == "facet") {
                    Triangle tri;
                    ss >> token; // "normal"
                    ss >> tri.normal.x >> tri.normal.y >> tri.normal.z;
                    std::getline(file, line); // outer loop
                    
                    std::getline(file, line); // vertex 1
                    std::stringstream v1ss(line);
                    v1ss >> token >> tri.v1.x >> tri.v1.y >> tri.v1.z;

                    std::getline(file, line); // vertex 2
                    std::stringstream v2ss(line);
                    v2ss >> token >> tri.v2.x >> tri.v2.y >> tri.v2.z;

                    std::getline(file, line); // vertex 3
                    std::stringstream v3ss(line);
                    v3ss >> token >> tri.v3.x >> tri.v3.y >> tri.v3.z;

                    std::getline(file, line); // endloop
                    std::getline(file, line); // endfacet
                    triangles.push_back(tri);
                }
            }
        } else {
            // --- Binary STL Parsing ---
            std::cout << "  Detected Binary STL format." << std::endl;
            char header[80] = "";
            file.read(header, 80);
            
            uint32_t num_triangles = 0;
            file.read(reinterpret_cast<char*>(&num_triangles), sizeof(uint32_t));

            for (uint32_t i = 0; i < num_triangles; ++i) {
                Triangle tri;
                tri.normal = read_point(file);
                tri.v1 = read_point(file);
                tri.v2 = read_point(file);
                tri.v3 = read_point(file);
                char dummy[2];
                file.read(dummy, 2);
                triangles.push_back(tri);
            }
        }
        
        if(triangles.empty()) {
             throw std::runtime_error("No triangles were read from the STL file.");
        }

        return triangles;
    }
}

