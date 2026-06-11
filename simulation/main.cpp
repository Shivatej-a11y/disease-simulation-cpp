#include <iostream>

#include "simulation.h"

#include <string>

int main(int argc, char* argv[]) {
    std::string input_file = "disease_in.ini";

    if (argc > 1) {
        input_file = argv[1];
    }

    Simulation simulation(input_file);
    simulation.start();

    return 0;
}
