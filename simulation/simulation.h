#ifndef HPC_DISEASE_SIMULATION_H_
#define HPC_DISEASE_SIMULATION_H_

#include <random>
#include <string>
#include <vector>

#include "Disease.h"
#include "Population.h"

struct TimestepRecord {
    std::string population_name;
    int run;
    int infectious;
    int recovered;
    int susceptible;
    int vaccinated;
};

struct RunSummaryRecord {
    int total_steps;
    int susceptible;
    int recovered;
    int vaccinated;
};

class Simulation {
public:
    explicit Simulation(std::string in_file = "disease_in.ini");

    void start();

    bool load_config();

    void set_disease(const Disease& disease);
    void add_population(const Population& population);
    void clear_populations();

    void set_simulation_runs(int runs);
    int get_simulation_runs() const;

    void run(int max_steps = 10000);

    bool write_disease_details_csv(const std::string& output_file = "disease_details.csv") const;
    bool write_disease_stats_csv(const std::string& output_file = "disease_stats.csv") const;

    const Disease& get_disease() const;

    int get_total_steps() const;
    int get_population_count() const;
    int get_details_record_count() const;
    int get_summary_record_count() const;

    int count_susceptible() const;
    int count_infectious() const;
    int count_recovered() const;
    int count_vaccinated() const;

    bool has_infectious() const;

private:
    std::string input_file;
    Disease disease;

    std::vector<Population> initial_populations;
    std::vector<Population> populations;

    int simulation_runs;
    int total_steps;

    std::mt19937 rng;
    std::vector<TimestepRecord> details_records;
    std::vector<RunSummaryRecord> summary_records;

    void run_single(int run_number, int max_steps);
    void record_current_state(int run_number);
    void record_run_summary();

    static double calculate_mean(const std::vector<int>& values);
    static double calculate_standard_deviation(const std::vector<int>& values);
};

#endif // HPC_DISEASE_SIMULATION_H_