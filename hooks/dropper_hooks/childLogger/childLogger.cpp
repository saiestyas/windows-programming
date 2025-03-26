// childLogger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return -1;
    }

    std::ofstream file("C:\\ProgramData\\drooper_hooks\\dropper.logs", ios::app);

    if (!file) {
        cout << "[!] Error opening log file C:\\ProgramData\\drooper_hooks\\dropper.logs" << endl;
        return -1;
    }

    
    file << argv[1] << endl;
    file.close();
    

    return 0;
}


