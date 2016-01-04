#include <iostream>
#include <string>
#include "mpi.h"
//#include "MPILifeSolver.h"
//#include <mpi.h>
#define mymessage 99
//
using namespace std;
//
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
 
    int i= (rank+1)*5;
    cout << i << endl;
    bool* buf = new bool[2];
//    printf("\nBefore sending: i= %d on %d\n", i, rank);
    if (rank==0) {
//        cout << i + 10 << "\n";
        cin >> i;
        cerr << "end" << endl;
        MPI_Send(&i, 1, MPI_INT, 1, mymessage, MPI_COMM_WORLD);
    }
    if (rank==1) {
        int j;
//        cerr << "start" << endl;
        cin >> j;
//        cerr << "end" << endl;
//        cout << "j" << j << "\n";
        MPI_Recv(buf,1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }
//    if (rank == 0) {
//        MPILifeSolver solver;
//        solver.run(size);
//    } else {
//        Worker worker;
//        worker.worker_function(rank, size);
//    }
    
    MPI_Finalize();
    return 0;
}

