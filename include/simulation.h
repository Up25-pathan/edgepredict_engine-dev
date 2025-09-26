#ifndef SIMULATION_H
#define SIMULATION_H

#include "json.hpp"
#include <vector>
#include <string>
#include <Eigen/Dense> // --- NEW: Include the Eigen library ---

using json = nlohmann::json;

// FEA Data Structures (remain the same)
struct Node {
    int id;
    double x, y, z;
    double temperature;
    double stress;
};

struct Element {
    int id;
    std::vector<int> node_ids;
};

// --- FEA Solver Class (Updated) ---
class FEASolver {
public:
    FEASolver(std::vector<Node>& nodes, std::vector<Element>& elements);
    void solve_time_step(double strain_increase, const json& material_params);

private:
    std::vector<Node>& mesh_nodes;
    std::vector<Element>& mesh_elements;

    // --- NEW: Eigen data structures for the solver ---
    // These will store the large system of equations for the entire mesh.
    Eigen::MatrixXd global_stiffness_matrix;
    Eigen::VectorXd force_vector;
    Eigen::VectorXd displacement_vector;

    void assemble_and_solve_system();
};


class Simulation {
public:
    Simulation();
    json run(const json& input_data);

private:
    std::vector<Node> nodes;
    std::vector<Element> elements;
    void load_mesh_from_stl(const std::string& filename);
};

#endif // SIMULATION_H

