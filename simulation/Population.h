#pragma once

#include <string>
#include <vector>

#include "Person.h"

class Population {
public:
    Population();
    Population(const std::string& name, int size, double vaccination_rate, bool patient_zero);

    std::string get_name() const;
    int get_size() const;
    double get_vaccination_rate() const;
    bool has_patient_zero() const;

    int count_susceptible() const;
    int count_infectious() const;
    int count_recovered() const;
    int count_vaccinated() const;

    bool has_infectious() const;

private:
    std::string name;
    int size;
    double vaccination_rate;
    bool patient_zero;
    std::vector<Person> persons;

    void initialize();
};