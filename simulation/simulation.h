#ifndef HPC_DISEASE_SIMULATION_H_
#define HPC_DISEASE_SIMULATION_H_

#include <random>
#include <string>
#include <vector>

#include "Disease.h"
#include "Population.h"

class Simulation {
public:
    explicit Simulation(std::string in_file = "disease_in.ini");

    void start();

    void set_disease(const Disease& disease);
    void add_population(const Population& population);
    void clear_populations();

    void run(int max_steps = 10000);

    int get_total_steps() const;
    int get_population_count() const;

    int count_susceptible() const;
    int count_infectious() const;
    int count_recovered() const;
    int count_vaccinated() const;

    bool has_infectious() const;

private:
    std::string input_file;
    Disease disease;
    std::vector<Population> populations;
    int total_steps;
    std::mt19937 rng;
};

#endif // HPC_DISEASE_SIMULATION_H_