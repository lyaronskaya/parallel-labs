#ifndef Parallel_5sem_Handler_h
#define Parallel_5sem_Handler_h

struct Handler {
    virtual void handle(void* arg) = 0;
    virtual ~Handler() {}
};

#endif
