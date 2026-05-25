#include "simulation.h"

#include <iostream>

Person::Person() : status(Status::SUSCEPTIBLE), days_remaining(0) {}

bool Person::is_infectious() const { return status == Status::INFECTIOUS; }
bool Person::is_susceptible() const { return status == Status::SUSCEPTIBLE; }
bool Person::is_recovered() const { return status == Status::RECOVERED; }
bool Person::is_vaccinated() const { return status == Status::VACCINATED; }

Simulation::Simulation(std::string in_file) { input_file = in_file; }

Disease::Disease(int duration, double transmissibility)
    : duration(duration), transmissibility(transmissibility) {}

int Disease::getDuration() const { return duration; }
double Disease::getTransmissibility() const { return transmissibility; }

void Simulation::start() { std::cout << "Starting simulation..." << std::endl; }
