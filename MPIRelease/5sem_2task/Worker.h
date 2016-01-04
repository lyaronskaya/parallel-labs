#ifndef WORKER_H
#define WORKER_H

#include <mpi.h>
#include "Field.h"
#include "CommandException.h"
#include "utility.h"

enum MESSAGE_TYPE {
    START,
    RUN,
    STOP,
    FINISH,
    FIELD_INFO,
    FIELD_INIT,
    FIELD_FULL,
    FIELD_GATHER,
    ITERATION_GATHER,
    ROW_SENDRECV
};

class Worker {
public:
    void worker_function(int rank, int comm_size);
    
private:
    int id;
    int workersCount;
    Field* field;
    Field* prev_field;
    bool* field_buffer;
    int field_height;
    int field_width;
    int iterations_todo;
    int iterations_ready;
    MPI_Request stop_request;
    int count_live_neighbors(int x, int y);
    void perform_field(bool* received_low_row, bool* received_high_row);
    bool check_break_work();
    
};

void Worker::worker_function(int rank, int comm_size) {
//    cout << "worker " << rank << "Started ";
    id = rank;
    workersCount = comm_size - 1;
    iterations_todo = 0;
    iterations_ready = 0;
    int worker_arg[2];
    MPI_Status status;
//    cout << "worker " << rank << "started receive field info\n";
    MPI_Recv(worker_arg, 2, MPI::INT, 0, FIELD_INFO, MPI_COMM_WORLD, &status);
//    cout << "worker " << rank << "ended receive field info\n";
    field_width = worker_arg[0] + 2;
    field_height = worker_arg[1];
    field_buffer = new bool[field_height * (field_width + 2)];
    
    MPI_Recv(field_buffer + field_height, field_height * field_width, MPI::BOOL, 0, FIELD_INIT, MPI_COMM_WORLD, &status);
    
    field->init_from_buffer(field_buffer, field_width + 2, field_height);
    prev_field->init_from_buffer(field_buffer, field_width + 2, field_height);
    
    int lower_worker_id = (rank == 1) ? (comm_size - 1) : (rank - 1);
    int higher_worker_id = (rank == comm_size - 1) ? 1 : (rank + 1);
    bool* lower_row_send = new bool[field_height];
    bool* higher_row_send = new bool[field_height];
    bool* lower_row_receive = new bool[field_height];
    bool* higher_row_receive = new bool[field_height];
    
    while(true) {
        bool curr_status = check_break_work();
        if (curr_status)
            break;
        
        field->write_row(lower_row_send, 1);
        MPI_Sendrecv(lower_row_send, field_height, MPI::BOOL, lower_worker_id, ROW_SENDRECV,
                     higher_row_receive, field_height, MPI::BOOL, higher_worker_id, ROW_SENDRECV, MPI_COMM_WORLD, &status);
        
        if (status.MPI_TAG == STOP) {
            int received_iteration = int_from_boolarray(higher_row_receive, field_height);
            if (received_iteration <= iterations_ready) {
                iterations_todo = 0;
                iterations_ready = received_iteration;
                continue;
            }
            MPI_Recv(higher_row_receive, field_height, MPI::BOOL, higher_worker_id, ROW_SENDRECV,
                     MPI_COMM_WORLD, &status);
        }
        
        field->write_row(higher_row_send, field_height - 2);
        MPI_Sendrecv(higher_row_send, field_height, MPI::BOOL, higher_worker_id, ROW_SENDRECV,
                     lower_row_receive, field_height, MPI::BOOL, lower_worker_id, ROW_SENDRECV, MPI_COMM_WORLD, &status);
        
        if (status.MPI_TAG == STOP) {
            int received_iteration = int_from_boolarray(lower_row_receive, field_height);
            if (received_iteration <= iterations_ready) {
                iterations_todo = 0;
                iterations_ready = received_iteration;
                continue;
            }
            MPI_Recv(lower_row_receive, field_height, MPI::BOOL, lower_worker_id, ROW_SENDRECV,
                     MPI_COMM_WORLD, &status);
        }
        
        perform_field(lower_row_receive, higher_row_receive);
        iterations_todo--;
        iterations_ready++;
    }
}

bool Worker::check_break_work() {
    int message;
    int flag = false;
    bool wait_stop = false;
    bool after_stop = false;
    MPI_Status status;
    
    while (iterations_todo <= 0) {
        MPI_Recv(&message, 1, MPI::INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        switch (status.MPI_TAG) {
            case RUN:
                iterations_todo += message;
                after_stop = true;
                return true;
            case FINISH:
                return false;
            case FIELD_GATHER:
                field->write_to_buffer(field_buffer, 0, field_width);
                MPI_Send(field_buffer, field_width * field_height, MPI::BOOL, 0, FIELD_GATHER, MPI_COMM_WORLD);
                break;
        }
    }
    
    if (id == 1 && !after_stop) {
        if (!wait_stop) {
            MPI_Recv(&message, 1, MPI::INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            wait_stop = true;
        }
        MPI_Test(&stop_request, &flag, &status);
        if (flag) {
            wait_stop = false;
            if (iterations_todo > 0) {
                return true;
            }
            iterations_todo += 2;
            for (int i = 2; i < workersCount; ++i) {
                bool iteration_buf[field_height];
                boolarray_from_int(iterations_ready, iteration_buf, field_height);
                MPI_Send(iteration_buf, field_height, MPI::BOOL, i, STOP, MPI_COMM_WORLD);
            }
        }
        after_stop = true;
    }
    
    return true;
}

void Worker::perform_field(bool* received_low_row, bool* received_high_row) {
    for (int i = 0; i < field_height; ++i) {
        field->data[0][i] = received_low_row[i];
        field->data[field_width - 1][i] = received_high_row[i];
    }
    int p, live_nbs;
    
    for (int i = 1; i < field->width - 1; ++i) {
        for (int j = 0; j < field->height; ++j) {
            p = prev_field->data[i][j];
            live_nbs = count_live_neighbors(i, j);
            if (p == 1) {
                if (live_nbs < 2 || live_nbs > 3)
                    field->data[i][j] = 0;
            } else {
                if (live_nbs == 3)
                    field->data[i][j] = 1;
            }
        }
    }
    std::swap(field, prev_field);
}

int Worker::count_live_neighbors(int x, int y) {
    unsigned int count = 0;
    for (int i = x - 1; i <= x+1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            if (i == x && j == y)
                continue;
            if (prev_field->data[i][j]) {
                count++;
            }
        }
    }
    return count;
}

#endif