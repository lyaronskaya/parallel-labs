#include <iostream>
#include <string>
#include "mpi.h"
#include "MPILifeSolver.h"

#define mymessage 99

using namespace std;

int main(int argc, char** argv) {
    MPI_Status status;
    MPI_Init(&argc, &argv);

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size < 2) {
        throw "Very few workers are available.";
    }
    
    if (rank == 0) {
        MPILifeSolver solver = MPILifeSolver();
        solver.run(size);
    } else {
        Worker worker = Worker();
        worker.worker_function(rank, size);
    }
    
    MPI_Finalize();
    return 0;
}








