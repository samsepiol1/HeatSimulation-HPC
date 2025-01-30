# Parallel Heat Diffusion Simulation

## Overview
This project implements a **2D heat diffusion simulation** using **MPI (Message Passing Interface)** to demonstrate the capabilities of **High Performance Computing (HPC)** in solving computationally intensive problems. The simulation is based on the heat equation, a mathematical model that describes the distribution of heat (or variation in temperature) over a given area:

# Heat Equation

The two-dimensional heat equation is given by:

$$
\frac{\partial u}{\partial t} = \alpha \left( \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2} \right)
$$

where:
- \( u \) is the temperature at a given point.
- \( \alpha \) is the thermal diffusivity constant.
- \( x, y \) are the spatial coordinates.
- \( t \) is time.

onde:
- \( u \) é a temperatura em um ponto dado.
- \( \alpha \) é a constante de difusividade térmica.
- \( x, y \) são as coordenadas espaciais.
- \( t \) é o tempo.

The simulation starts with an initial condition where one edge of the grid is heated (hotline), and the heat propagates across the grid over time. The primary goal is to compute the temperature at each grid point efficiently.

---

## Problem Statement
The **heat diffusion problem** models various real-world scenarios such as:
- Heat transfer in solids.
- Temperature distribution in a room.
- Simulation of thermal processes in engineering.

As the size of the grid and the number of time steps increase, the computational cost grows significantly. Solving this problem efficiently requires distributing the workload across multiple processors.

---

## Solution Approach
### Parallelization
This project uses **MPI** to parallelize the computation:
1. **Domain Decomposition**:
   - The 2D grid is divided into smaller subgrids, each handled by a separate process.
   - Each process updates its portion of the grid based on the heat equation.

2. **Communication**:
   - Processes exchange boundary data with their neighbors to ensure continuity in the heat diffusion.

3. **Aggregation**:
   - The final temperature grid is gathered and reconstructed by the master process.

### Benefits of Parallelization
- **Scalability**: The computation is distributed across multiple processors, reducing the execution time.
- **Efficiency**: Communication overhead is minimized by limiting data exchanges to adjacent processes.
- **Flexibility**: The solution supports different grid sizes and numbers of processes.

---

## How It Helps
By solving the heat diffusion problem in parallel, this project:
1. Demonstrates the use of HPC techniques for computational science problems.
2. Provides a foundation for solving other partial differential equations in parallel.
3. Highlights the importance of parallel programming models like MPI in handling large-scale problems.

The solution is designed to be modular and adaptable, allowing researchers and students to extend it to more complex scenarios or optimize it further for specific architectures.


## Code Explanation

### Main Components

1. **Initialization**:
   The grid is initialized with:
   - A heated row at the top (temperature = 100).
   - All other cells set to 0.

   
   ```c++
   void initialize_grid(std::vector<std::vector<double>> &grid, int rank, int size) {
       for (int i = 0; i < grid.size(); ++i) {
           for (int j = 0; j < grid[0].size(); ++j) {
               if (rank == 0 && i == 0) {
                   grid[i][j] = 100.0; // Hotline
               } else {
                   grid[i][j] = 0.0;
               }
           }
       }
   }


2. **Computation**

    ```c++
    void update_grid(std::vector<std::vector<double>> &current, std::vector<std::vector<double>> &next) {
    for (int i = 1; i < current.size() - 1; ++i) {
        for (int j = 1; j < current[0].size() - 1; ++j) {
            next[i][j] = current[i][j] +
                         ALPHA * (current[i - 1][j] + current[i + 1][j] +
                                  current[i][j - 1] + current[i][j + 1] - 4 * current[i][j]);
        }
    }
}


3. Parallelization:

- The grid is divided into subgrids, each assigned to a process.
- Neighboring processes exchange boundary rows using MPI_Send and MPI_Recv.


4. Final Aggregation

- Subgrids are gathered in the master process to form the final grid.
- Results are saved to output_grid.txt.


## How to execute


### Compilation

To compile the code, use the following command:

```bash

mpicxx -o heat_simulation heat_simulation.cpp


```


### Execution
Run the program with mpirun, specifying the number of processes:

```bash
mpirun -np 4 ./heat_simulation

```
or


```bash
mpirun --oversubscribe -np 4 ./heat_simulation

```


### Results

- The program will save the final temperature grid to output_grid.txt.
- If enabled, the program will also print the grid to the terminal.


## Example output

File output_grid.txt (partial example for a 10x10 grid):

100.0 100.0 100.0 100.0 100.0 100.0 100.0 100.0 100.0 100.0 
75.0  75.0  75.0  75.0  75.0  75.0  75.0  75.0  75.0  75.0 
50.0  50.0  50.0  50.0  50.0  50.0  50.0  50.0  50.0  50.0 
...


![Heat Simulation Example](https://i.gifer.com/VS42.gif)


