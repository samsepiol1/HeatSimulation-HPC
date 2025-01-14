#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#define GRID_SIZE 100
#define TIME_STEPS 500
#define ALPHA 0.01

// Inicializa a grade com a linha inicial quente no processo mestre
void initialize_grid(std::vector<std::vector<double>> &grid, int rank, int size) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[0].size(); ++j) {
            if (rank == 0 && i == 0) {
                grid[i][j] = 100.0; // Linha inicial quente
            } else {
                grid[i][j] = 0.0;
            }
        }
    }
}

// Atualiza a grade com base na equação de difusão
void update_grid(std::vector<std::vector<double>> &current, std::vector<std::vector<double>> &next) {
    for (int i = 1; i < current.size() - 1; ++i) {
        for (int j = 1; j < current[0].size() - 1; ++j) {
            next[i][j] = current[i][j] +
                         ALPHA * (current[i - 1][j] + current[i + 1][j] +
                                  current[i][j - 1] + current[i][j + 1] - 4 * current[i][j]);
        }
    }
}

// Salva a grade completa em um arquivo
void save_grid_to_file(const std::vector<std::vector<double>> &grid, int rank) {
    if (rank == 0) {
        std::ofstream file("output_grid.txt");
        for (const auto &row : grid) {
            for (const auto &cell : row) {
                file << cell << " ";
            }
            file << std::endl;
        }
        file.close();
    }
}

// Imprime a grade no terminal (apenas para depuração)
void print_grid(const std::vector<std::vector<double>> &grid, int rank) {
    if (rank == 0) {
        for (const auto &row : grid) {
            for (const auto &cell : row) {
                std::cout << cell << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "---------------------------------------" << std::endl;
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Divisão das linhas entre processos
    int local_rows = GRID_SIZE / size;
    std::vector<std::vector<double>> current_grid(local_rows + 2, std::vector<double>(GRID_SIZE, 0.0));
    std::vector<std::vector<double>> next_grid = current_grid;

    initialize_grid(current_grid, rank, size);

    for (int t = 0; t < TIME_STEPS; ++t) {
        update_grid(current_grid, next_grid);
        current_grid.swap(next_grid);

        // Comunicação entre processos
        if (rank > 0) {
            MPI_Send(&current_grid[1][0], GRID_SIZE, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&current_grid[0][0], GRID_SIZE, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Recv(&current_grid[local_rows + 1][0], GRID_SIZE, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&current_grid[local_rows][0], GRID_SIZE, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
    }

    // Reunir a grade completa no processo mestre
    if (rank == 0) {
        std::vector<std::vector<double>> full_grid(GRID_SIZE, std::vector<double>(GRID_SIZE, 0.0));
        for (int i = 1; i <= local_rows; ++i) {
            full_grid[i - 1] = current_grid[i];
        }
        for (int p = 1; p < size; ++p) {
            MPI_Recv(&full_grid[p * local_rows][0], local_rows * GRID_SIZE, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        save_grid_to_file(full_grid, rank); // Salva a grade completa em arquivo
        print_grid(full_grid, rank);       // Opcional: Imprime a grade no terminal
    } else {
        MPI_Send(&current_grid[1][0], local_rows * GRID_SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
