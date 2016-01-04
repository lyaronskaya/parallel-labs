#ifndef MPIHANDLER_H
#define MPIHANDLER_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <utility>
#include <cassert>
#include <mpi.h>
#include "Handler.h"

using namespace std;
//
//struct HandlerArg {
//    Field* life_field;
//    Master* master;
//};

struct StartHandler : public Handler {
    void handle(Master* arg) {
//        cout << "start command\n";
        Master* master = (Master*)arg;
        std::string field_info;
        
        cin >> field_info;
        
        try {
            int w = stoi(field_info), h;
            cin >> h;
            cout << master->temp << endl;
            master->life_field->init_random(w, h);
            cout << "init random ended\n";
        }
        catch(...) {
            cerr << "init from file\n";
            master->life_field->init_from_file(field_info);
        }
        cout << "Master starts init workers\n";
        master->init_workers();
        cout << "Master ends init workers\n";
        master->change_state(Master::NOT_RUNNING);
    }
};

struct StatusHandler : public Handler {
    void handle(void* arg) {
        Master* master = (Master*)arg;
        if (master->get_state() == Master::NOT_STARTED) {
            cout << "The system has not yet started.";
            return;
        }
        if (master->get_state() == Master::RUNNING) {
            cout << "The system is still running. Try again.";
            return;
        }
        master->gather_field();
        cout << "Iteration number " << master->get_iter_number();
        master->get_field()->print_world();
    }
};

struct RunHandler : public Handler {
    void handle(void* arg) {
        Master* master = (Master*)arg;
        int iteration_count;
        cin >> iteration_count;
        master->run_workers(iteration_count);
        master->change_state(Master::RUNNING);
    }
};

struct StopHandler : public Handler {
    void handle(void* arg) {
        Master* master = (Master*)arg;
        if (master->get_state() == Master::NOT_STARTED) {
            cout << "The system has not yet started.";
            return;
        }
        master->stop_workers();
        master->change_state(Master::NOT_RUNNING);
    }
};

struct QuitHandler : public Handler {
    void handle(void* arg) {
        Master* master = (Master*)arg;
        master->shutdown();
        exit(EXIT_SUCCESS);
    }
};

#endif