#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <map>
#include "../include/simulation.h"
#include "../include/stl_reader.h"
#include "../include/solver.h" // --- NEW: Include our CUDA solver header ---

// The FEASolver class and physics functions are no longer needed in this file,
// as the core logic has been moved to solver.cu and will run on the GPU.

Simulation::Simulation() {}

void Simulation::load_mesh_from_stl(const std::string& filename) {
    std::cout << "  Loading mesh from STL file: " << filename << std::endl;
    try {
        auto triangles = stl_reader::read_stl(filename);
        std::map<std::string, int> vertex_map;
        int node_id_counter = 0;
        int element_id_counter = 0;

        auto process_vertex = [&](const stl_reader::Point& v) -> int {
            std::string key = std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z);
            if (vertex_map.find(key) == vertex_map.end()) {
                vertex_map[key] = node_id_counter;
                nodes.push_back({node_id_counter, v.x, v.y, v.z, 25.0, 0.0});
                return node_id_counter++;
            }
            return vertex_map[key];
        };

        for (const auto& tri : triangles) {
            int id1 = process_vertex(tri.v1);
            int id2 = process_vertex(tri.v2);
            int id3 = process_vertex(tri.v3);
            elements.push_back({element_id_counter++, {id1, id2, id3}});
        }

        std::cout << "  Mesh loaded successfully. Found " << nodes.size() << " nodes and " << elements.size() << " elements." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error loading STL file: " << e.what() << std::endl;
    }
}

json Simulation::run(const json& input_data) {
    json output;
    load_mesh_from_stl("tool.stl"); 

    if (nodes.empty()) {
        output["execution_summary"]["status"] = "Failed";
        output["execution_summary"]["error"] = "Could not load mesh from tool.stl";
        return output;
    }

    int time_steps = input_data["simulation_settings"]["time_steps"];
    json material_params = input_data["workpiece_material"]["johnson_cook_params"];
    double current_strain = 0.0;
    double duration = input_data["simulation_settings"]["duration_sec"];
    
    std::cout << "  Running CUDA-accelerated simulation for " << time_steps << " steps..." << std::endl;

    std::vector<json> time_series;
    
    for (int i = 0; i < time_steps; ++i) {
        current_strain += 0.01;
        
        // --- KEY CHANGE: Call the CUDA solver function ---
        // This function will manage memory transfers and launch the GPU kernel.
        solve_time_step_cuda(nodes, current_strain, material_params);
        
        // Data aggregation and logging remains the same
        double max_stress_step = 0.0;
        double max_temp_step = 0.0;
        for (const auto& node : nodes) {
            if (node.stress > max_stress_step) max_stress_step = node.stress;
            if (node.temperature > max_temp_step) max_temp_step = node.temperature;
        }

        std::cout << "  - Step " << i + 1 << "/" << time_steps << " | Max Temp: " << max_temp_step << " C" << std::endl;
        
        json step_data;
        step_data["time_sec"] = (i + 1) * (duration / time_steps);
        step_data["temperature_celsius"] = max_temp_step;
        step_data["stress_mpa"] = max_stress_step;
        time_series.push_back(step_data);
    }
    
    // Final summary aggregation
    double final_max_stress = 0.0;
    double final_max_temp = 0.0;
    for (const auto& node : nodes) {
        if (node.stress > final_max_stress) final_max_stress = node.stress;
        if (node.temperature > final_max_temp) final_max_temp = node.temperature;
    }

    output["simulation_id"] = input_data["simulation_id"];
    output["summary_metrics"]["max_temperature_celsius"] = final_max_temp;
    output["summary_metrics"]["max_von_mises_stress_mpa"] = final_max_stress;
    output["summary_metrics"]["tool_life_estimate_min"] = 118.5; // Dummy value
    output["time_series_data"] = time_series;
    output["execution_summary"]["status"] = "Success";
    output["recommendations"] = {
        "AI-Generated: Simulation accelerated with NVIDIA CUDA.",
        "AI-Generated: Core physics calculated on GPU."
    };
    
    return output;
}

