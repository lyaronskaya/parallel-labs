#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>
#include <vector>
#include <ctime>
//#include <libiomp/omp.h>
#include <omp.h>
#include <stdio.h>
#include <stdexcept>

using namespace std;

typedef vector<vector<int> > field_t;

field_t field;
field_t prev_field;
int num_threads;
int per_thread;
int field_width, field_height;
bool break_work = false;
omp_lock_t go_work_lock;
omp_lock_t iter_lock;
vector<omp_lock_t> row_locks;

int iter_number, iter_todo;

void print_world();
int count_live_neighbors(int x, int y);
void copy_field(int first, int last);
int read_from_csv(string path, field_t& field);

enum StateType
{
    NOT_STARTED,
    NOT_RUNNING,
    RUNNING,
    STOPPED,
    FINISHED
};

class IncorrectCommandException: public std::runtime_error {
public:
    explicit IncorrectCommandException(const std::string& what):
    std::runtime_error (what)
    {}
};

struct WorkerArg {
    int first;
    int last;
    int id;
};

void perform_field(int first, int last, int thread_id) {
    int p, live_nbs;
    for (int i = first; i < last; ++i) {
        if (i == first) {
            omp_set_lock(&row_locks[(thread_id - 1 + num_threads) % num_threads]);
        } else if (i == last - 1) {
            omp_set_lock(&row_locks[thread_id]);
        }

        for (int j = 0; j < field_height; ++j) {
            p = prev_field[i][j];
            live_nbs = count_live_neighbors(i, j);
            if (p == 1) {
                if (live_nbs < 2 || live_nbs > 3)
                    field[i][j] = 0;
            } else {
                if (live_nbs == 3)
                    field[i][j] = 1;
            }
        }
        if (i == first) {
            omp_unset_lock(&row_locks[(thread_id - 1 + num_threads) % num_threads]);
        } else if (i == last - 1) {
            omp_unset_lock(&row_locks[thread_id]);
        }
    }
}


void worker_func(WorkerArg* arg) {
#pragma omp critical
    cout << "Started " << omp_get_thread_num() << endl;
#pragma omp master
    {
//        cout << "master\n";
        if (iter_todo <= 0) {
            omp_set_lock(&go_work_lock);
        }
    }
    while(true) {
        if (break_work) {
//#pragma omp critical
//            cout << "Ended " << omp_get_thread_num() << endl;
            return;
        }
#pragma omp master
        {
            
            while (iter_todo <= 0);
//                cout << "iter todo <=0\n";
            omp_unset_lock(&go_work_lock);
        }
        while (!omp_test_lock (&go_work_lock))
            sleep(2);
        omp_unset_lock(&go_work_lock);
        
        perform_field(arg->first, arg->last, arg->id);
        copy_field(arg->first, arg->last-1);

        if (omp_get_thread_num() == 1) {
            int per_thread = arg->last - arg->first;
            for (int i = 0; i < num_threads; ++i) {
                int locked_line_num = per_thread * (i + 1) - 1;
                copy_field(locked_line_num, locked_line_num + 1);
            }
        }
#pragma omp master
        {
//            cout << "decrease\n";
            iter_todo--;
            iter_number++;
            if (iter_todo <= 0) {
                omp_set_lock(&go_work_lock);
            }
        }
#pragma omp barrier
    }
//#pragma omp critical
//    cout << "Ended " << omp_get_thread_num() << endl;
}

struct Handler {
    virtual void handle(StateType& state) = 0;
    virtual ~Handler() {}
};

struct StartHandler : public Handler {
    
    void handle(StateType& state) {
        
//        cout << "start\n";
#pragma omp master
        {
            string arg1, file_name;
            cin >> arg1;
            try {
                field_width = stoi(arg1);
                cin >> field_height;
                field = vector<vector<int> >(field_width, vector<int>(field_height));
            
                for (int i = 0; i < field_width; ++i) {
                    for (int j = 0; j < field_height; ++j) {
                        field[i][j] = rand() % 2;
                    }
                }
            }
            catch(...) {
                file_name = arg1;
                read_from_csv(file_name, field);
                field_width = field.size();
                field_height = field[0].size();
            }
            prev_field = vector<vector<int> >(field_width, vector<int>(field_height));
            copy_field(0, field_width);
            cin >> num_threads;
            per_thread = field_width / num_threads;
            row_locks.resize(num_threads);
            for (int i = 0; i < num_threads; ++i) {
                omp_init_lock(&row_locks[i]);
            }
        }
#pragma omp single
        {
            state = NOT_RUNNING;
        }
//        cout << "thread_num in start before create workers " << omp_get_thread_num() << endl;
//        cout << "per thread " << per_thread << endl;
        omp_set_nested(1);
        if (omp_get_thread_num() == 1) {
#pragma omp parallel num_threads(num_threads)
            {
                WorkerArg* arg = new WorkerArg;
                int id = omp_get_thread_num();
                arg->first = id * per_thread;
                arg->last = arg->first + per_thread;
                arg->id = id;
                worker_func(arg);
            }
        }
    }
};

struct StatusHandler : public Handler {
    void handle(StateType& state) {
//#pragma omp master
//        {
//        cout << iter_todo << endl;
//        }
        if (state == NOT_STARTED) {
#pragma omp master
            cout << "The system has not yet started.\n";
            return;
        }
        if (iter_todo <= 0) {
#pragma omp master
            state = NOT_RUNNING;
        }
        if (state == RUNNING) {
#pragma omp master
            cout << iter_todo << " The system is still running. Try again.\n";
            return;
        }
#pragma omp master
        {
            cout << iter_number << endl;
            for (int i = 0; i < field_width; ++i) {
                for (int j = 0; j < field_height; ++j) {
                    if (field[i][j]) {
                        cout << "*";
                    }
                    else cout << " ";
                }
                cout << "\n";
            }
        }
    }
};

struct RunHandler : public Handler {
    void handle(StateType& state) {
        int iter_num;
#pragma omp master
        {
            cin >> iter_num;
            omp_set_lock(&iter_lock);
            iter_todo += iter_num;
            omp_unset_lock(&iter_lock);
        }
#pragma omp master
        {
//            omp_unset_lock(&run_lock);
            state = RUNNING;
        }
    }
};

struct StopHandler : public Handler {
    void handle(StateType& state) {
        if (state == NOT_STARTED) {
#pragma omp master
            cout << "The system has not yet started.\n";
            return;
        }
#pragma omp master
        {
#pragma omp atomic
            iter_todo &= 0;
        }

    }
};

struct QuitHandler : public Handler {
    void handle(StateType& state) {
#pragma omp master
        {
#pragma omp atomic
            break_work &= 1;
            for (int i = 0; i < num_threads; i++) {
                omp_destroy_lock(&row_locks[i]);
            }
        omp_destroy_lock(&go_work_lock);
        }
        exit(EXIT_SUCCESS);
    }
};

struct LifeSolver
{
    std::map<std::string, Handler*> handlers;
    StateType state;
    
    LifeSolver() {
        handlers["START"] = new StartHandler();
        handlers["STATUS"] = new StatusHandler();
        handlers["RUN"] = new RunHandler();
        handlers["STOP"] = new StopHandler();
        handlers["QUIT"] = new QuitHandler();
    }
    
    void run() {
        state = NOT_STARTED;
        omp_init_lock(&go_work_lock);
        omp_init_lock(&iter_lock);
        omp_set_nested(1);
        string command;
        
#pragma omp parallel num_threads(2)
        {
            while (true) {
#pragma omp master
                {
                    cout << "$ ";
                    cin >> command;
                }

                if (handlers[command]) {
                    handlers[command]->handle(state);
                }
                else {
#pragma omp master
                    cout << "Wrong command\n";
                }
            }
        }
        
    }
};

int read_from_csv(string path, field_t& field) {
    try {
        ifstream csv(path);
        string line;
//        field = vector<vector<bool>>();
        for (int i = 0; getline(csv, line); ++i) {
            istringstream iss(line);
            string s;
            field.push_back(vector<int>());
            while(getline(iss, s, ';'))
            {
                if (s == "1" || s == "#")
                    field[i].push_back(1);
                else {
                    if (s == "0" || s == ".")
                        field[i].push_back(0);
                else {
                    throw IncorrectCommandException("Incorrect data. Use only 0 . 1 # ");
                }
                }
            }
            for (int j = 0; j < field_height; ++j) {
                cout << field[i][j] << " ";

            }
            cout << "\n";
        }
    } catch (exception& e) {
        static const string str("While reading csv: ");
        throw IncorrectCommandException(str + e.what());
    }
    return 0;
}

int main(int argc, char *argv[])
{
    LifeSolver solver;
    solver.run();
    return 0;
}


int count_live_neighbors(int x, int y) {
    unsigned int count = 0;
    for (int i = x - 1; i <= x+1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            if (i == x && j == y)
                continue;
            if (prev_field[(i + field_width) % field_width][(j + field_height) % field_height]) {
                count++;
            }
        }
    }
    return count;
}

void copy_field(int first, int last) {
    for (int i = first; i < last; ++i) {
        for (int j = 0; j < field_height; ++j) {
            prev_field[i][j] = field[i][j];
        }
    }
}




