#ifndef LIFESOLVER_H
#define LIFESOLVER_H

#include <string>
#include <map>
#include <iostream>
#include "Handler.h"
#include "Field.h"

using namespace std;

class LifeSolver
{
    Field* life_field;
public:
    std::map<std::string, Handler*> handlers;
    void run() {
        
        while (true) {
            cout << "$ ";
            
            string command;
            
            cin >> command;
            if (handlers[command]) {
                handlers[command]->handle(life_field);
            }
            else {
                cout << "Wrong command\n";
            }
        }
        
    }
};

#endif
