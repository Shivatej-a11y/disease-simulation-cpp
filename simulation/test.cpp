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