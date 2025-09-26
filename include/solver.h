#ifndef SOLVER_H
#define SOLVER_H

#include "simulation.h"

// This is the "wrapper" function that C++ will call.
// It's responsible for managing memory transfers to and from the GPU
// and launching the CUDA kernel.
void solve_time_step_cuda(std::vector<Node>& nodes, double current_strain, const json& material_params);

#endif // SOLVER_H
