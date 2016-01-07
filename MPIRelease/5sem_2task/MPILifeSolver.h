#ifndef MPILIFESOLVER_H
#define MPILIFESOLVER_H

#include "LifeSolver.h"
#include "Field.h"
#include "Master.h"
#include "MPIHandler.h"

using namespace std;

class MPILifeSolver: public LifeSolver {
    Master* master;
public:
    MPILifeSolver();
    ~MPILifeSolver();
    void run(int size);
};

MPILifeSolver::MPILifeSolver() {
    handlers["START"] = new StartHandler();
    handlers["STATUS"] = new StatusHandler();
    handlers["RUN"] = new RunHandler();
    handlers["STOP"] = new StopHandler();
    handlers["QUIT"] = new QuitHandler();
}

void MPILifeSolver::run(int size) {
    master = new Master();
    master->set_workers_count(size - 1);
    while (true) {
        cout << "$ ";
        string command;
        cin >> command;
        if (handlers[command]) {
            handlers[command]->handle(master);
        }
        else {
            cout << "Wrong command\n";
        }
    }
}

MPILifeSolver::~MPILifeSolver() {
    assert(master->get_state() == Master::NOT_STARTED || master->get_state() == Master::FINISHED);
}

#endif
