#include "simulation.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "INIReader.h"

namespace {
std::string make_output_path(const std::string& input_file, const std::string& output_name) {
    std::filesystem::path input_path(input_file);

    if (input_path.has_parent_path()) {
        return (input_path.parent_path() / output_name).string();
    }

    return output_name;
}
}

Simulation::Simulation(std::string in_file)
    : input_file(in_file),
      disease(),
      initial_populations(),
      populations(),
      simulation_runs(1),
      total_steps(0),
      rng(42),
      details_records(),
      summary_records() {}

void Simulation::start() {
    std::cout << "Starting simulation..." << std::endl;

    if (!load_config()) {
        std::cerr << "Failed to load configuration from " << input_file << std::endl;
        return;
    }

    run();
    write_disease_details_csv(make_output_path(input_file, "disease_details.csv"));
    write_disease_stats_csv(make_output_path(input_file, "disease_stats.csv"));

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

    simulation_runs =
        static_cast<int>(reader.GetInteger("global", "simulation_runs", 1));

    if (simulation_runs < 1) {
        simulation_runs = 1;
    }

    std::string disease_name = reader.Get("disease", "name", "unknown");
    int disease_duration =
        static_cast<int>(reader.GetInteger("disease", "duration", 0));
    double transmissibility =
        reader.GetReal("disease", "transmissibility", 0.0);

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
    summary_records.clear();
    rng.seed(42);

    return true;
}

void Simulation::set_disease(const Disease& new_disease) {
    disease = new_disease;
}

void Simulation::add_population(const Population& population) {
    initial_populations.push_back(population);
    populations.push_back(population);
}

void Simulation::clear_populations() {
    initial_populations.clear();
    populations.clear();
    total_steps = 0;
    details_records.clear();
    summary_records.clear();
}

void Simulation::set_simulation_runs(int runs) {
    simulation_runs = std::max(1, runs);
}

int Simulation::get_simulation_runs() const {
    return simulation_runs;
}

void Simulation::run(int max_steps) {
    total_steps = 0;
    details_records.clear();
    summary_records.clear();

    for (int run_number = 0; run_number < simulation_runs; ++run_number) {
        run_single(run_number, max_steps);
    }
}

void Simulation::run_single(int run_number, int max_steps) {
    populations = initial_populations;
    total_steps = 0;

    rng.seed(42 + run_number);

    record_current_state(run_number);

    while (has_infectious() && total_steps < max_steps) {
        for (Population& population : populations) {
            population.simulate_one_day(disease, rng);
        }

        total_steps++;
        record_current_state(run_number);
    }

    record_run_summary();
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

void Simulation::record_run_summary() {
    RunSummaryRecord record;

    record.total_steps = total_steps;
    record.susceptible = count_susceptible();
    record.recovered = count_recovered();
    record.vaccinated = count_vaccinated();

    summary_records.push_back(record);
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

bool Simulation::write_disease_stats_csv(const std::string& output_file) const {
    std::ofstream file(output_file);

    if (!file.is_open()) {
        return false;
    }

    std::vector<int> total_steps_values;
    std::vector<int> susceptible_values;
    std::vector<int> recovered_values;
    std::vector<int> vaccinated_values;

    for (const RunSummaryRecord& record : summary_records) {
        total_steps_values.push_back(record.total_steps);
        susceptible_values.push_back(record.susceptible);
        recovered_values.push_back(record.recovered);
        vaccinated_values.push_back(record.vaccinated);
    }

    file << "key,mean,std\n";

    file << "total_steps,"
         << calculate_mean(total_steps_values) << ","
         << calculate_standard_deviation(total_steps_values) << "\n";

    file << "susceptiple_persons,"
         << calculate_mean(susceptible_values) << ","
         << calculate_standard_deviation(susceptible_values) << "\n";

    file << "recovered_persons,"
         << calculate_mean(recovered_values) << ","
         << calculate_standard_deviation(recovered_values) << "\n";

    file << "vaccinated_persons,"
         << calculate_mean(vaccinated_values) << ","
         << calculate_standard_deviation(vaccinated_values) << "\n";

    return true;
}

double Simulation::calculate_mean(const std::vector<int>& values) {
    if (values.empty()) {
        return 0.0;
    }

    double sum = 0.0;

    for (int value : values) {
        sum += value;
    }

    return sum / static_cast<double>(values.size());
}

double Simulation::calculate_standard_deviation(const std::vector<int>& values) {
    if (values.empty()) {
        return 0.0;
    }

    double mean = calculate_mean(values);
    double squared_difference_sum = 0.0;

    for (int value : values) {
        double difference = static_cast<double>(value) - mean;
        squared_difference_sum += difference * difference;
    }

    return std::sqrt(squared_difference_sum / static_cast<double>(values.size()));
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

int Simulation::get_summary_record_count() const {
    return static_cast<int>(summary_records.size());
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