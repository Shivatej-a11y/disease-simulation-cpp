#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"

#include "./Person.h"
#include "./Disease.h"
#include "./simulation.h"

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