#ifndef MASTER_H
#define MASTER_H

#include "Worker.h"
#include "CommandException.h"

class Master {
public:
    enum StateType
    {
        NOT_STARTED,
        NOT_RUNNING,
        RUNNING,
        STOPPED,
        FINISHED
    };
    
    Field* life_field;
    
private:
    StateType state;
    unsigned int workersCount;
    bool* field_buffer;
    int worker_arg[2];
    int iterNumber;
    
public:
    Master() : state(NOT_STARTED) { life_field = new Field; }
    void init_workers();
    void run_workers(int iterations);
    void stop_workers();
    void shutdown();
    void gather_field();
    void change_state(StateType s);
    StateType get_state();
    int get_iter_number();
    Field* get_field();
    void set_workers_count(int workers_count);
    int get_workers_count();
};

void Master::set_workers_count(int workers_count) {
    workersCount = workers_count;
}

int Master::get_workers_count() {
    return workersCount;
}

void Master::init_workers() {
    field_buffer = new bool[life_field->width * life_field->height];
    worker_arg[0] = life_field->width / workersCount;
    worker_arg[1] = life_field->height;
    cout << worker_arg[0] << " " << worker_arg[2] << " " << workersCount << endl;
    int start_row = 0;
    
    for (int i = 1; i <= workersCount; ++i) {
        if (i == workersCount) {
            worker_arg[0] += life_field->width % workersCount;
        }
        life_field->write_to_buffer(field_buffer, start_row, worker_arg[0]);
        
        MPI_Send(worker_arg, 2, MPI::INT, i, FIELD_INFO, MPI_COMM_WORLD);
        MPI_Send(field_buffer, worker_arg[0] * life_field->height, MPI::BOOL,
                 i, FIELD_INIT, MPI_COMM_WORLD);
        cout << "sended info\n";
        start_row += worker_arg[0];
    }
}

void Master::gather_field() {
    MPI_Status status;
    bool* field_part_pointer = field_buffer;
    bool some_message = 0;
    worker_arg[0] = life_field->width / workersCount;

    for (int i = workersCount; i >= 1; --i) {
        if (i == workersCount) {
            worker_arg[0] += life_field->width % workersCount;
        }
        
        MPI_Send(&some_message, 1, MPI::INT, i, FIELD_GATHER, MPI_COMM_WORLD);
        MPI_Recv(field_part_pointer, worker_arg[0] * life_field->height, MPI::BOOL, i, FIELD_GATHER, MPI_COMM_WORLD, &status);
        field_part_pointer += worker_arg[0] * life_field->height;
    }
    life_field->init_from_buffer(field_buffer, life_field->width, life_field->height);
}

int Master::get_iter_number() {
    return iterNumber;
}

Field* Master::get_field() {
    return life_field;
}

void Master::run_workers(int iterations) {
    for (int i = 1; i <= workersCount; ++i) {
        MPI_Send(&iterations, 1, MPI::INT, i, RUN, MPI_COMM_WORLD);
    }
}

void Master::change_state(StateType s) {
    state = s;
}

Master::StateType Master::get_state() {
    return state;
}

void Master::stop_workers() {
    MPI_Status status;
    bool some_message = 0;
    cout << "Master starts stopping workers\n";
    MPI_Send(&some_message, 1, MPI::INT, 1, STOP, MPI_COMM_WORLD);
    cout << "Master sent some info\n";
    int new_iter_number;
    int iterations_ready;
    cout << "But previous iterNumber " << iterNumber << endl;
    for (int i = 2; i <= workersCount; ++i) {
        MPI_Recv(&iterations_ready, 1, MPI::INT, i, ITERATION_GATHER, MPI_COMM_WORLD, &status);
        cout << "received from " << i << " " << iterations_ready << endl;
    }
    iterNumber = iterations_ready;
}

void Master::shutdown() {
    int some_message = 0;
    for (int i = 1; i <= workersCount; ++i) {
        MPI_Send(&some_message, 1, MPI::INT, i, FINISH, MPI_COMM_WORLD);
    }
    state = FINISHED;
}

#endif
