#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"

#include "./Person.h"
#include "./Disease.h"
#include "./simulation.h"
#include "./Population.h"

TEST_CASE("Test simulation class") {
    Simulation sim = Simulation();
}

TEST_CASE("Person starts as susceptible") {
    Person person;

    CHECK(person.is_susceptible());
    CHECK_FALSE(person.is_infectious());
    CHECK_FALSE(person.is_recovered());
    CHECK_FALSE(person.is_vaccinated());
}

TEST_CASE("Susceptible person can be infected") {
    Person person;

    person.infect();

    CHECK(person.is_infectious());
    CHECK_FALSE(person.is_susceptible());
}

TEST_CASE("Vaccinated person cannot be infected") {
    Person person;

    person.vaccinate();
    person.infect();

    CHECK(person.is_vaccinated());
    CHECK_FALSE(person.is_infectious());
}

TEST_CASE("Recovered person cannot be infected again") {
    Person person;

    person.infect();
    person.recover();
    person.infect();

    CHECK(person.is_recovered());
    CHECK_FALSE(person.is_infectious());
}

TEST_CASE("Infectious person recovers after disease duration") {
    Person person;

    person.infect();

    person.progress_one_day(3);
    CHECK(person.is_infectious());

    person.progress_one_day(3);
    CHECK(person.is_infectious());

    person.progress_one_day(3);
    CHECK(person.is_recovered());
}

TEST_CASE("Disease stores configured values") {
    Disease disease("Corona", 5, 0.1);

    CHECK(disease.get_name() == "Corona");
    CHECK(disease.get_duration() == 5);
    CHECK(disease.get_transmissibility() == doctest::Approx(0.1));
}

TEST_CASE("Disease has default values") {
    Disease disease;

    CHECK(disease.get_name() == "unknown");
    CHECK(disease.get_duration() == 0);
    CHECK(disease.get_transmissibility() == doctest::Approx(0.0));
}

TEST_CASE("Population stores configured values") {
    Population population("Deggendorf", 100, 0.3, false);

    CHECK(population.get_name() == "Deggendorf");
    CHECK(population.get_size() == 100);
    CHECK(population.get_vaccination_rate() == doctest::Approx(0.3));
    CHECK_FALSE(population.has_patient_zero());
}

TEST_CASE("Population initializes correct number of vaccinated persons") {
    Population population("Deggendorf", 10, 0.3, false);

    CHECK(population.count_vaccinated() == 3);
    CHECK(population.count_susceptible() == 7);
    CHECK(population.count_infectious() == 0);
    CHECK(population.count_recovered() == 0);
}

TEST_CASE("Population initializes patient zero when enabled") {
    Population population("Deggendorf", 10, 0.2, true);

    CHECK(population.count_vaccinated() == 2);
    CHECK(population.count_infectious() == 1);
    CHECK(population.count_susceptible() == 7);
    CHECK(population.count_recovered() == 0);
    CHECK(population.has_infectious());
}

TEST_CASE("Population without patient zero has no infectious persons") {
    Population population("Deggendorf", 10, 0.2, false);

    CHECK(population.count_vaccinated() == 2);
    CHECK(population.count_infectious() == 0);
    CHECK(population.count_susceptible() == 8);
    CHECK_FALSE(population.has_infectious());
}

TEST_CASE("Fully vaccinated population does not create patient zero") {
    Population population("Deggendorf", 10, 1.0, true);

    CHECK(population.count_vaccinated() == 10);
    CHECK(population.count_infectious() == 0);
    CHECK(population.count_susceptible() == 0);
    CHECK_FALSE(population.has_infectious());
}

TEST_CASE("Population handles empty population") {
    Population population("Empty", 0, 0.5, true);

    CHECK(population.get_size() == 0);
    CHECK(population.count_vaccinated() == 0);
    CHECK(population.count_infectious() == 0);
    CHECK(population.count_susceptible() == 0);
    CHECK(population.count_recovered() == 0);
    CHECK_FALSE(population.has_infectious());
}