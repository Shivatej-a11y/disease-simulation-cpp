#include "simulation.h"

#include <fstream>
#include <iostream>
#include <string>

#include "INIReader.h"

Simulation::Simulation(std::string in_file)
    : input_file(in_file),
      disease(),
      populations(),
      total_steps(0),
      rng(42),
      details_records() {}

void Simulation::start() {
    std::cout << "Starting simulation..." << std::endl;

    if (!load_config()) {
        std::cerr << "Failed to load configuration from " << input_file << std::endl;
        return;
    }

    run();
    write_disease_details_csv();

    std::cout << "Number of timesteps until disease extinction: "
              << total_steps << std::endl;
    std::cout << "Number of vaccinated persons: "
              << count_vaccinated() << std::endl;
    std::cout << "Number of recovered persons: "
              << count_recovered() << std::endl;
    std::cout << "Number of susceptible persons: "
              << count_susceptible() << std::endl;
}

bool Simulation::load_config() {
    INIReader reader(input_file);

    if (reader.ParseError() < 0) {
        std::cerr << "Could not open or parse config file: " << input_file << std::endl;
        return false;
    }

    clear_populations();

    std::string disease_name = reader.Get("disease", "name", "unknown");
    int disease_duration = static_cast<int>(reader.GetInteger("disease", "duration", 0));
    double transmissibility = reader.GetReal("disease", "transmissibility", 0.0);

    disease = Disease(disease_name, disease_duration, transmissibility);

    int number_of_populations =
        static_cast<int>(reader.GetInteger("global", "num_populations", 0));

    for (int i = 1; i <= number_of_populations; ++i) {
        std::string section_name = "population_" + std::to_string(i);

        std::string population_name =
            reader.Get(section_name, "name", section_name);

        int population_size =
            static_cast<int>(reader.GetInteger(section_name, "size", 0));

        double vaccination_rate =
            reader.GetReal(section_name, "vaccination_rate", 0.0);

        bool patient_zero =
            reader.GetBoolean(section_name, "patient_0", false);

        Population population(
            population_name,
            population_size,
            vaccination_rate,
            patient_zero
        );

        add_population(population);
    }

    total_steps = 0;
    details_records.clear();
    rng.seed(42);

    return true;
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
    details_records.clear();
}

void Simulation::run(int max_steps) {
    total_steps = 0;
    details_records.clear();

    const int run_number = 0;

    record_current_state(run_number);

    while (has_infectious() && total_steps < max_steps) {
        for (Population& population : populations) {
            population.simulate_one_day(disease, rng);
        }

        total_steps++;
        record_current_state(run_number);
    }
}

void Simulation::record_current_state(int run_number) {
    for (const Population& population : populations) {
        TimestepRecord record;

        record.population_name = population.get_name();
        record.run = run_number;
        record.infectious = population.count_infectious();
        record.recovered = population.count_recovered();
        record.susceptible = population.count_susceptible();
        record.vaccinated = population.count_vaccinated();

        details_records.push_back(record);
    }
}

bool Simulation::write_disease_details_csv(const std::string& output_file) const {
    std::ofstream file(output_file);

    if (!file.is_open()) {
        return false;
    }

    file << "name,run,infectious,recovered,susceptiple,vaccinated\n";

    for (const TimestepRecord& record : details_records) {
        file << record.population_name << ","
             << record.run << ","
             << record.infectious << ","
             << record.recovered << ","
             << record.susceptible << ","
             << record.vaccinated << "\n";
    }

    return true;
}

const Disease& Simulation::get_disease() const {
    return disease;
}

int Simulation::get_total_steps() const {
    return total_steps;
}

int Simulation::get_population_count() const {
    return static_cast<int>(populations.size());
}

int Simulation::get_details_record_count() const {
    return static_cast<int>(details_records.size());
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