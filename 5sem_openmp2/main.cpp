#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
//#include <semaphore.h>
//#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <ctime>
//#include <libiomp/omp.h>
#include <omp.h>
#include <stdio.h>
//#include </opt/local/lib/gcc49/gcc/x86_64-apple-darwin14/4.9.3/include/omp.h>

using namespace std;

const int MAX_THREADS = 5;
int NUM_THREADS;
//pthread_t threads[MAX_THREADS];
//pthread_t master;

typedef vector<vector<int> > field_t;
field_t field;
field_t prev_field;
int FIELD_WIDTH, FIELD_HEIGHT;
//pthread_mutex_t row_locks[MAX_THREADS];
bool break_work = false;
//pthread_cond_t go_work;
//pthread_mutex_t work_mutex;
//pthread_mutex_t end_mutex;
//sem_t* iter_ready;
//
//pthread_cond_t run;
//pthread_mutex_t run_mutex;
//pthread_mutex_t iter_todo_mutex;

int iter_number, iter_todo;

void print_world();
int count_live_neighbors(int x, int y);
void copy_field(int first, int last);
int read_from_csv(string path, field_t& field);

class IncorrectCommandException: public std::runtime_error {
public:
    explicit IncorrectCommandException(const std::string& what):
    std::runtime_error(what)
    {}
};

void perform_field(int first, int last, int mutex_num) {
    int p, live_nbs;
    for (int i = first; i < last; ++i) {
//        if (i == first) {
//            pthread_mutex_lock(&row_locks[(mutex_num - 1 + NUM_THREADS) % NUM_THREADS]);
//        } else if (i == last - 1) {
//            pthread_mutex_lock(&row_locks[mutex_num]);
//        }
        for (int j = 0; j < FIELD_HEIGHT; ++j) {
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
//        if (i == first) {
//            pthread_mutex_unlock(&row_locks[(mutex_num - 1 + NUM_THREADS) % NUM_THREADS]);
//        } else if (i == last - 1) {
//            pthread_mutex_unlock(&row_locks[mutex_num]);
//        }
    }
}


void* worker_func(void* argv) {
    int *args = (int *)argv;
    int first = args[0];
    int last = args[1];
    int thread_num = args[2];
    
    while(true) {
//        pthread_mutex_lock(&end_mutex);
//        if (break_work) {
//            pthread_mutex_unlock(&end_mutex);
//            return NULL;
//        }
//        pthread_mutex_unlock(&end_mutex);
//        
//        pthread_mutex_lock(&work_mutex);
//        pthread_cond_wait(&go_work, &work_mutex);
//        pthread_mutex_unlock(&work_mutex);
//        
//        perform_field(first, last, thread_num);
//        copy_field(first, last-1);
//        
//        int status = sem_wait(iter_ready);
//        if (status != 0) {
//            perror("sem_wait failed");
//            exit(status);
//        }
    }
    return NULL;
}

void* master_func(void* args) {
    int num_threads = ((int *)args)[0];
    int per_thread = ((int *)args)[1];
//    while (true) {
//        pthread_mutex_lock(&end_mutex);
//        if (break_work) {
//            pthread_mutex_unlock(&end_mutex);
//            for (int i = 0; i < NUM_THREADS; ++i) {
//                pthread_join(threads[i], NULL);
//            }
//            return NULL;
//        }
//        pthread_mutex_unlock(&end_mutex);
//        
//        pthread_mutex_lock(&run_mutex);
//        while (iter_todo == 0 && !break_work)
//            pthread_cond_wait(&run, &run_mutex);
//        pthread_mutex_unlock(&run_mutex);
//        
//        pthread_mutex_lock(&work_mutex);
//        pthread_cond_signal(&go_work);
//        pthread_mutex_unlock(&work_mutex);
//        
//        for (int i = 0; i < num_threads; ++i) {
//            sem_post(iter_ready);
//        }
//        
//        for (int i = 0; i < num_threads; ++i) {
//            int locked_line_num = per_thread * (i + 1) - 1;
//            copy_field(locked_line_num, locked_line_num + 1);
//        }
//        
//        iter_number++;
//        pthread_mutex_lock(&iter_todo_mutex);
//        iter_todo--;
//        pthread_mutex_unlock(&iter_todo_mutex);
//    }
    return NULL;
}

struct Handler {
    virtual void handle() = 0;
    virtual ~Handler() {}
};

struct StartHandler : public Handler {
    
    void handle() {
        
        string arg1, file_name;
        int N, M;
        
        cin >> arg1;
        
        try {
            N = stoi(arg1);
            cin >> M;
            
            FIELD_WIDTH = N;
            FIELD_HEIGHT = M;
            field = vector<vector<int> >(FIELD_WIDTH, vector<int>(FIELD_HEIGHT));
            
            for (int i = 0; i < M; ++i) {
                for (int j = 0; j < M; ++j) {
                    field[i][j] = rand() % 2;
                }
            }
        }
        catch(...) {
            file_name = arg1;
            read_from_csv(file_name, field);
            FIELD_WIDTH = field.size();
            FIELD_HEIGHT = field[0].size();
        }
        prev_field = vector<vector<int> >(FIELD_WIDTH, vector<int>(FIELD_HEIGHT));
        copy_field(0, FIELD_WIDTH);
        
        //        int num_threads =  (FIELD_WIDTH / 30 < MAX_THREADS)? FIELD_WIDTH / 30 : MAX_THREADS;
        //        if ((num_threads > MAX_THREADS && (FIELD_WIDTH % 30)) || num_threads == 0)
        //            num_threads++;
        //        NUM_THREADS = num_threads;
        cin >> NUM_THREADS;
        int per_thread = FIELD_WIDTH / NUM_THREADS;
//        
//        iter_ready = sem_open("/iter_ready", O_CREAT, 0600, NUM_THREADS);
//        
//        for (int i = 0; i < NUM_THREADS; ++i) {
//            row_locks[i] = PTHREAD_MUTEX_INITIALIZER;
//            int args[3];
//            args[0] = i*per_thread;
//            args[1] = args[0] + per_thread;
//            args[2] = i;
//            int result = pthread_create(&threads[i], NULL, worker_func, &args);
//            if (result != 0) {
//                perror("Failed creating thread");
//                exit(EXIT_FAILURE);
//            }
//        }
        
        int args[3];
        args[0] = NUM_THREADS;
        args[1] = per_thread;
//        int result = pthread_create(&master, NULL, master_func, &args);
//        if (result != 0) {
//            perror("Failed creating thread");
//            exit(EXIT_FAILURE);
//        }
    }
};

struct StatusHandler  : public Handler {
    void handle() {
        printf("%d\n", iter_number);
        for (int i = 0; i < FIELD_WIDTH; ++i) {
            for (int j = 0; j < FIELD_HEIGHT; ++j) {
                if (field[i][j]) {
                    printf("*");
                }
                else printf(" ");
            }
            printf("\n");
        }
    }
};

struct RunHandler  : public Handler {
    void handle() {
        int iter_num;
        cin >> iter_num;
//        pthread_mutex_lock(&iter_todo_mutex);
//        iter_todo += iter_num;
//        pthread_mutex_unlock(&iter_todo_mutex);
//        pthread_cond_signal(&run);
    }
};

struct StopHandler  : public Handler {
    void handle() {
//        pthread_mutex_lock(&iter_todo_mutex);
//        iter_todo = 0;
//        pthread_mutex_unlock(&iter_todo_mutex);
    }
};

struct QuitHandler  : public Handler {
    void handle() {
//        pthread_mutex_lock(&end_mutex);
//        break_work = true;
//        pthread_mutex_unlock(&end_mutex);
//        
//        pthread_join(master, NULL);
        exit(EXIT_SUCCESS);
    }
};

struct LifeSolver
{
    std::map<std::string, Handler*> handlers;
    
    LifeSolver() {
        handlers["START"] = new StartHandler();
        handlers["STATUS"] = new StatusHandler();
        handlers["RUN"] = new RunHandler();
        handlers["STOP"] = new StopHandler();
        handlers["QUIT"] = new QuitHandler();
    }
    
    void run() {
        
        while (true) {
            cout << "$ ";
            
            string command;
            
            cin >> command;
            if (handlers[command]) {
                handlers[command]->handle();
            }
            else {
                cout << "Wrong command\n";
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
                else {if (s == "0" || s == ".")
                    field[i].push_back(0);
                else {
                    throw IncorrectCommandException("Incorrect data. Use only 0 . 1 # ");
                }
                }
            }
            for (auto x: field[i]) {
                cout << x << " ";
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
    int n;
    
    omp_set_num_threads(10);
    cout << omp_get_max_threads();
#pragma omp parallel
    {
        n = 1;
        printf("%d\n", omp_get_max_threads() );
#pragma omp master
        {
            n = 2;
        }
        
        printf("Первое значение n: %d\n", n);
        
#pragma omp barrier
        
#pragma omp master
        {
            n = 3;
        }
        
        printf("Второе значение n: %d\n", n);
    }

//    LifeSolver solver;
//    solver.run();
//    // "/Users/luba_yaronskaya/Documents/Parallel_5sem/Parallel_5sem/data/dataset.csv"
//    return 0;
}

void print_world()
{
    for (int i = 0; i < FIELD_WIDTH; ++i) {
        for (int j = 0; j < FIELD_HEIGHT; ++j) {
            if (field[i][j] == 1)
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }
}

int count_live_neighbors(int x, int y) {
    unsigned int count = 0;
    for (int i = x - 1; i <= x+1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            if (i == x && j == y)
                continue;
            if (prev_field[(i + FIELD_WIDTH) % FIELD_WIDTH][(j + FIELD_HEIGHT) % FIELD_HEIGHT]) {
                count++;
            }
        }
    }
    return count;
}

void copy_field(int first, int last) {
    for (int i = first; i < last; ++i) {
        for (int j = 0; j < FIELD_HEIGHT; ++j) {
            prev_field[i][j] = field[i][j];
        }
    }
}




