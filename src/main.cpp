#include <iostream>
#include <fstream>
#include <string>
#include "../include/simulation.h"
#include "../include/json.hpp"

// For convenience
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.json>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::cout << "--- EdgePredict Core Simulation Engine ---" << std::endl;

    // 1. Read Input JSON
    std::cout << "Reading input from: " << input_filename << std::endl;
    std::ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return 1;
    }
    json input_data;
    input_file >> input_data;

    // 2. Run Simulation
    std::cout << "Starting simulation for ID: " << input_data["simulation_id"] << std::endl;
    Simulation sim;
    json output_data = sim.run(input_data);
    std::cout << "Simulation completed successfully." << std::endl;

    // 3. Write Output JSON
    std::string output_filename = "output.json";
    std::cout << "Writing results to: " << output_filename << std::endl;
    std::ofstream output_file(output_filename);
    output_file << output_data.dump(4); // pretty print with 4 spaces

    std::cout << "--- Engine finished ---" << std::endl;

    return 0;
}
