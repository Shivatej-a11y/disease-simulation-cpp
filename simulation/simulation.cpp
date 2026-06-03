#include "simulation.h"

#include <iostream>

Simulation::Simulation(std::string in_file)
    : input_file(in_file),
      disease(),
      populations(),
      total_steps(0),
      rng(42) {}

void Simulation::start() {
    std::cout << "Starting simulation..." << std::endl;
    run();

    std::cout << "Number of timesteps until disease extinction: "
              << total_steps << std::endl;
    std::cout << "Number of vaccinated persons: "
              << count_vaccinated() << std::endl;
    std::cout << "Number of recovered persons: "
              << count_recovered() << std::endl;
    std::cout << "Number of susceptible persons: "
              << count_susceptible() << std::endl;
}

void Simulation::set_disease(const Disease& new_disease) {
    disease = new_disease;
}

void Simulation::add_population(const Population& population) {
    populations.push_back(population);
}

void Simulation::clear_populations() {
    populations.clear();
    total_steps = 0;
}

void Simulation::run(int max_steps) {
    total_steps = 0;

    while (has_infectious() && total_steps < max_steps) {
        for (Population& population : populations) {
            population.simulate_one_day(disease, rng);
        }

        total_steps++;
    }
}

int Simulation::get_total_steps() const {
    return total_steps;
}

int Simulation::get_population_count() const {
    return static_cast<int>(populations.size());
}

int Simulation::count_susceptible() const {
    int count = 0;

    for (const Population& population : populations) {
        count += population.count_susceptible();
    }

    return count;
}

int Simulation::count_infectious() const {
    int count = 0;

    for (const Population& population : populations) {
        count += population.count_infectious();
    }

    return count;
}

int Simulation::count_recovered() const {
    int count = 0;

    for (const Population& population : populations) {
        count += population.count_recovered();
    }

    return count;
}

int Simulation::count_vaccinated() const {
    int count = 0;

    for (const Population& population : populations) {
        count += population.count_vaccinated();
    }

    return count;
}

bool Simulation::has_infectious() const {
    return count_infectious() > 0;
}