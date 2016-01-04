//#include <iostream>
//#include <string>
//#include "MPILifeSolver.h"
//#include <mpi.h>
//#define mymessage 99
//
//using namespace std;
//
//int main(int argc, char** argv) {
//    MPI_Status status;
//    MPI_Comm NewComm;
//    MPI_Init(&argc, &argv);
////    if (status != 0) {
////        throw "MPI init error.";
////    }
//    MPI_Comm_dup(MPI_COMM_WORLD, &NewComm);
//    MPI_Keyval_create(MPI_DUP_FN, MPI_NULL_DELETE_FN, &IO_KEY, extra_arg);
//    
//    int rank;
//    int size;
//    
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//    
//    if (size < 2) {
//        throw "Very few workers are available.";
//    }
// 
//    int i= (rank+1)*5;
//    bool* buf = new bool[2];
////    printf("\nBefore sending: i= %d on %d\n", i, rank);
//    if (rank==0) {
////        cout << i + 10 << "\n";
//        cin >> i;
//        cerr << "end" << endl;
//        MPI_Send(&i, 1, MPI_INT, 1, mymessage, MPI_COMM_WORLD);
//    }
//    if (rank==1) {
//        int j;
////        cerr << "start" << endl;
//        cin >> j;
////        cerr << "end" << endl;
////        cout << "j" << j << "\n";
//        MPI_Recv(buf,1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//    }
////    if (rank == 0) {
////        MPILifeSolver solver;
////        solver.run(size);
////    } else {
////        Worker worker;
////        worker.worker_function(rank, size);
////    }
//    
//    MPI_Finalize();
//    return 0;
//}

#include <stdio.h>
#include "mpi.h"
int main(int argc, char** argv) {
    int size, rank, numb;
    int key;
    int *value;
    void* extra_arg;
    void* io_rank_att;
    void* v;
    int vval, flag;
    int* io_rank;
    *io_rank = 0;
    MPI_Comm NewComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &NewComm);
    MPI_Comm_free(&NewComm);
//    MPI_Keyval_create(MPI_DUP_FN, MPI_NULL_DELETE_FN, &key, extra_arg);
//
//    MPI_Attr_put(NewComm, key, io_rank);
//    MPI_Attr_get(NewComm, key, &io_rank_att, &flag);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//    MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_IO, &v, &flag);
//    std::cout << *(int*)v << " " << flag << std::endl;
//
//    MPI_Comm_dup(MPI_COMM_WORLD, &NewComm);
//    MPI_Keyval_create(MPI_DUP_FN, MPI_NULL_DELETE_FN, &key, extra_arg);
//
//    MPI_Attr_put(NewComm, key, io_rank);
//    MPI_Attr_get(NewComm, key, &io_rank_att, &flag);
    if (rank == 0) {
//        printf("enter an integer ");
//        scanf(" %d",&numb);
//        printf("%d", numb);
    }
//    MPI_Bcast(&numb, 1, MPI_INT, 0, MPI_COMM_WORLD);
//    printf("Hello world! I'm  %d  of  %d - numb = %d\n", rank, size,numb);
    MPI_Finalize();}
