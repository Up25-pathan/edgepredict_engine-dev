#include <iostream>
#include <cuda_runtime.h>
#include "../include/solver.h"

// --- Physics Functions (CPU versions for fallback/reference) ---
// These are the same Johnson-Cook and Heat Generation functions as before.
// ...

// --- The CUDA Kernel ---
// This is the function that will run in parallel on thousands of GPU cores.
// Each thread will be responsible for calculating the physics for one node.
__global__ void update_nodes_kernel(Node* d_nodes, int num_nodes, double current_strain, double A, double B, double n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < num_nodes) {
        Node node = d_nodes[idx]; // Get the node for this thread

        // Run the same physics calculations, but on the GPU
        if (node.z < 0.5) {
            const double MELTING_TEMP = 660.0, ROOM_TEMP = 25.0;
            const double DENSITY = 2700.0, SPECIFIC_HEAT = 896.0;
            double strain_increase_this_step = 0.01;

            double stress = node.stress; // Simplified Johnson-Cook on GPU
            double temp_increase = (stress * 1e6 * strain_increase_this_step * 0.9) / (DENSITY * SPECIFIC_HEAT);
            
            node.temperature += temp_increase;
            node.stress = stress; // Update stress if needed
        }
        
        d_nodes[idx] = node; // Write the updated node back to memory
    }
}

// --- The C++ Wrapper Function ---
void solve_time_step_cuda(std::vector<Node>& nodes, double current_strain, const json& material_params) {
    int num_nodes = nodes.size();
    if (num_nodes == 0) return;

    // 1. Allocate memory on the GPU
    Node* d_nodes; // Pointer to device (GPU) memory
    cudaMalloc(&d_nodes, num_nodes * sizeof(Node));

    // 2. Copy the node data from the CPU (host) to the GPU (device)
    cudaMemcpy(d_nodes, nodes.data(), num_nodes * sizeof(Node), cudaMemcpyHostToDevice);

    // 3. Launch the CUDA Kernel
    // We tell the GPU to launch enough threads to cover all our nodes.
    int threads_per_block = 256;
    int blocks_per_grid = (num_nodes + threads_per_block - 1) / threads_per_block;
    
    // Unpack material params for the kernel
    double A = material_params["A"];
    double B = material_params["B"];
    double n = material_params["n"];

    update_nodes_kernel<<<blocks_per_grid, threads_per_block>>>(d_nodes, num_nodes, current_strain, A, B, n);

    // 4. Copy the results back from the GPU to the CPU
    cudaMemcpy(nodes.data(), d_nodes, num_nodes * sizeof(Node), cudaMemcpyDeviceToHost);

    // 5. Free the memory on the GPU
    cudaFree(d_nodes);
}
