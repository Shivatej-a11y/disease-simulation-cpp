#include "Population.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

Population::Population()
    : name("unknown"),
      size(0),
      vaccination_rate(0.0),
      patient_zero(false),
      persons() {}

Population::Population(const std::string& name, int size, double vaccination_rate, bool patient_zero)
    : name(name),
      size(std::max(0, size)),
      vaccination_rate(std::clamp(vaccination_rate, 0.0, 1.0)),
      patient_zero(patient_zero),
      persons() {
    initialize();
}

void Population::initialize() {
    persons.clear();
    persons.resize(size);

    if (size <= 0) {
        return;
    }

    int patient_zero_index = -1;

    if (patient_zero) {
        patient_zero_index = 0;
        persons[patient_zero_index].infect();
    }

    int target_vaccinated_count =
        static_cast<int>(std::round(size * vaccination_rate));

    int max_vaccinated_count = size;

    if (patient_zero_index != -1) {
        max_vaccinated_count = size - 1;
    }

    target_vaccinated_count =
        std::min(target_vaccinated_count, max_vaccinated_count);

    std::vector<int> candidate_indices;

    for (int i = 0; i < size; ++i) {
        if (i != patient_zero_index) {
            candidate_indices.push_back(i);
        }
    }

    std::mt19937 vaccination_rng(1234);
    std::shuffle(candidate_indices.begin(), candidate_indices.end(), vaccination_rng);

    for (int i = 0; i < target_vaccinated_count; ++i) {
        persons[candidate_indices[i]].vaccinate();
    }
}

std::string Population::get_name() const {
    return name;
}

int Population::get_size() const {
    return size;
}

double Population::get_vaccination_rate() const {
    return vaccination_rate;
}

bool Population::has_patient_zero() const {
    return patient_zero;
}

int Population::count_susceptible() const {
    int count = 0;

    for (const Person& person : persons) {
        if (person.is_susceptible()) {
            count++;
        }
    }

    return count;
}

int Population::count_infectious() const {
    int count = 0;

    for (const Person& person : persons) {
        if (person.is_infectious()) {
            count++;
        }
    }

    return count;
}

int Population::count_recovered() const {
    int count = 0;

    for (const Person& person : persons) {
        if (person.is_recovered()) {
            count++;
        }
    }

    return count;
}

int Population::count_vaccinated() const {
    int count = 0;

    for (const Person& person : persons) {
        if (person.is_vaccinated()) {
            count++;
        }
    }

    return count;
}

bool Population::has_infectious() const {
    return count_infectious() > 0;
}

void Population::simulate_one_day(const Disease& disease, std::mt19937& rng) {
    if (size <= 0) {
        return;
    }

    std::vector<int> infectious_indices;

    for (int i = 0; i < size; ++i) {
        if (persons[i].is_infectious()) {
            infectious_indices.push_back(i);
        }
    }

    if (infectious_indices.empty()) {
        return;
    }

    std::uniform_int_distribution<int> person_distribution(0, size - 1);
    std::uniform_real_distribution<double> probability_distribution(0.0, 1.0);

    std::vector<int> newly_infected_indices;

    for (int infectious_index : infectious_indices) {
        for (int contact = 0; contact < CONTACTS_PER_DAY; ++contact) {
            int contacted_index = person_distribution(rng);

            if (size > 1) {
                while (contacted_index == infectious_index) {
                    contacted_index = person_distribution(rng);
                }
            }

            Person& contacted_person = persons[contacted_index];

            if (contacted_person.is_susceptible()) {
                double probability = probability_distribution(rng);

                if (probability < disease.get_transmissibility()) {
                    newly_infected_indices.push_back(contacted_index);
                }
            }
        }
    }

    for (int infectious_index : infectious_indices) {
        persons[infectious_index].progress_one_day(disease.get_duration());
    }

    for (int newly_infected_index : newly_infected_indices) {
        persons[newly_infected_index].infect();
    }
}