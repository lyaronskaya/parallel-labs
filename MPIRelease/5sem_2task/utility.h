#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
using namespace std;

int int_from_boolarray(bool* array, int size) {
    int res = 0;
    for (int i = 0; i < size; ++i) {
        res |= array[i] << (size - 1 - i);
        cout << res << "\n";
    }
    return res;
}

void boolarray_from_int(int n, bool* res, int size) {
    for (int i = size - 1; i >= 0; --i) {
        res[i] = n & 1;
        n >>= 1;
    }
}
#endif

