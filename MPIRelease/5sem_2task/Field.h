#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CommandException.h"

using namespace std;

class Field {
public:
    int width;
    int height;
    vector<vector<bool>> data;

    void init_random(size_t width, size_t height);
    void init_from_file(string file);
    void print_world();
    void write_to_buffer(bool* buffer, int start_row, int row_count);
    void init_from_buffer(bool* buffer, int width_, int height_);
    void write_row(bool* buffer, int row_number);
};

void Field::init_random(size_t width_, size_t height_) {
    width = width_;
    height = height_;
    
    data.resize(width, vector<bool>(height));
    
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            data[i][j] = rand() % 2;
        }
    }
}

void Field::init_from_file(string file) {
    try {
        ifstream csv(file);
        string line;
        data = vector<vector<bool>>();
        for (int i = 0; getline(csv, line); ++i) {
            istringstream iss(line);
            string s;
            data.push_back(vector<bool>());
            while(getline(iss, s, ';'))
            {
                if (s == "1" || s == "#")
                    data[i].push_back(1);
                else {if (s == "0" || s == ".")
                    data[i].push_back(0);
                else {
//                    throw IncorrectCommandException("Incorrect data. Use only 0 . 1 # ");
                }
                }
            }
        }
    } catch (exception& e) {
        static const string str("While reading csv: ");
        throw IncorrectCommandException(str + e.what());
    }
    width = data.size();
    height = data[0].size();
}

void Field::print_world()
{
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (data[i][j])
                cout << "*";
            else
                cout << " ";
        }
        cout << endl;
    }
}

void Field::write_to_buffer(bool* buffer, int start_row, int row_count) {
    int index = 0;
    for (int i = 0; i < row_count; ++i) {
        for (int j = 0; j < height; ++j, ++index) {
            buffer[index] = data[start_row + i][j];
        }
    }
}

void Field::init_from_buffer(bool* buffer, int width_, int height_) {
    width = width_;
    height = height_;
    data.resize(width, vector<bool>(height));
    int index = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j, ++index) {
            data[i][j] = buffer[index];
        }
    }
}

void Field::write_row(bool* buffer, int row_number) {
    for (int i = 0; i < height; ++i) {
        buffer[i] = data[row_number][i];
    }
}
#endif