#include "Person.h"

Person::Person()
    : status(Status::SUSCEPTIBLE), days_infected(0) {}

bool Person::is_susceptible() const {
    return status == Status::SUSCEPTIBLE;
}

bool Person::is_infectious() const {
    return status == Status::INFECTIOUS;
}

bool Person::is_recovered() const {
    return status == Status::RECOVERED;
}

bool Person::is_vaccinated() const {
    return status == Status::VACCINATED;
}

void Person::infect() {
    if (status == Status::SUSCEPTIBLE) {
        status = Status::INFECTIOUS;
        days_infected = 0;
    }
}

void Person::vaccinate() {
    if (status == Status::SUSCEPTIBLE) {
        status = Status::VACCINATED;
    }
}

void Person::recover() {
    if (status == Status::INFECTIOUS) {
        status = Status::RECOVERED;
    }
}

void Person::progress_one_day(int disease_duration) {
    if (status == Status::INFECTIOUS) {
        days_infected++;

        if (days_infected >= disease_duration) {
            recover();
        }
    }
}

Person::Status Person::get_status() const {
    return status;
}

int Person::get_days_infected() const {
    return days_infected;
}
