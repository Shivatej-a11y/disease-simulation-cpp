#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"

#include <cstdio>
#include <fstream>

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

TEST_CASE("Disease does not spread when transmissibility is zero") {
    Population population("Deggendorf", 10, 0.0, true);
    Disease disease("TestDisease", 5, 0.0);
    std::mt19937 rng(42);

    population.simulate_one_day(disease, rng);

    CHECK(population.count_infectious() == 1);
    CHECK(population.count_recovered() == 0);
    CHECK(population.count_susceptible() == 9);
}

TEST_CASE("Infectious person recovers after disease duration in population") {
    Population population("Deggendorf", 10, 0.0, true);
    Disease disease("ShortDisease", 1, 0.0);
    std::mt19937 rng(42);

    population.simulate_one_day(disease, rng);

    CHECK(population.count_infectious() == 0);
    CHECK(population.count_recovered() == 1);
    CHECK(population.count_susceptible() == 9);
}

TEST_CASE("Disease spreads to susceptible person with full transmissibility") {
    Population population("Deggendorf", 2, 0.0, true);
    Disease disease("HighlyInfectious", 5, 1.0);
    std::mt19937 rng(42);

    population.simulate_one_day(disease, rng);

    CHECK(population.count_infectious() == 2);
    CHECK(population.count_susceptible() == 0);
}

TEST_CASE("Fully vaccinated population does not spread disease") {
    Population population("Deggendorf", 10, 1.0, true);
    Disease disease("HighlyInfectious", 5, 1.0);
    std::mt19937 rng(42);

    population.simulate_one_day(disease, rng);

    CHECK(population.count_infectious() == 0);
    CHECK(population.count_vaccinated() == 10);
    CHECK(population.count_susceptible() == 0);
}

TEST_CASE("Recovered person is not infected again during population spread") {
    Person person;
    person.infect();
    person.recover();
    person.infect();

    CHECK(person.is_recovered());
    CHECK_FALSE(person.is_infectious());
}

TEST_CASE("Simulation stores disease and populations") {
    Simulation simulation;

    Disease disease("TestDisease", 5, 0.1);
    Population population("Deggendorf", 10, 0.2, true);

    simulation.set_disease(disease);
    simulation.add_population(population);

    CHECK(simulation.get_population_count() == 1);
    CHECK(simulation.count_vaccinated() == 2);
    CHECK(simulation.count_infectious() == 1);
    CHECK(simulation.count_susceptible() == 7);
    CHECK(simulation.has_infectious());
}

TEST_CASE("Simulation runs until disease extinction") {
    Simulation simulation;

    Disease disease("ShortDisease", 1, 0.0);
    Population population("Deggendorf", 10, 0.0, true);

    simulation.set_disease(disease);
    simulation.add_population(population);
    simulation.run();

    CHECK(simulation.get_total_steps() == 1);
    CHECK(simulation.count_infectious() == 0);
    CHECK(simulation.count_recovered() == 1);
    CHECK(simulation.count_susceptible() == 9);
    CHECK_FALSE(simulation.has_infectious());
}

TEST_CASE("Simulation does not run when there are no infectious persons") {
    Simulation simulation;

    Disease disease("TestDisease", 5, 0.1);
    Population population("Deggendorf", 10, 0.2, false);

    simulation.set_disease(disease);
    simulation.add_population(population);
    simulation.run();

    CHECK(simulation.get_total_steps() == 0);
    CHECK(simulation.count_infectious() == 0);
    CHECK_FALSE(simulation.has_infectious());
}

TEST_CASE("Simulation handles multiple populations") {
    Simulation simulation;

    Disease disease("ShortDisease", 1, 0.0);
    Population population_one("Deggendorf", 10, 0.0, true);
    Population population_two("Munich", 20, 0.5, false);

    simulation.set_disease(disease);
    simulation.add_population(population_one);
    simulation.add_population(population_two);

    CHECK(simulation.get_population_count() == 2);
    CHECK(simulation.count_vaccinated() == 10);
    CHECK(simulation.count_infectious() == 1);
    CHECK(simulation.count_susceptible() == 19);

    simulation.run();

    CHECK(simulation.get_total_steps() == 1);
    CHECK(simulation.count_infectious() == 0);
    CHECK(simulation.count_recovered() == 1);
}

TEST_CASE("Simulation loads configuration from ini file") {
    const std::string file_name = "test_disease_in.ini";

    std::ofstream config_file(file_name);

    config_file << "[global]\n";
    config_file << "simulation_name = test_simulation\n";
    config_file << "num_populations = 2\n";
    config_file << "simulation_runs = 1\n\n";

    config_file << "[disease]\n";
    config_file << "name = Corona\n";
    config_file << "duration = 5\n";
    config_file << "transmissibility = 0.1\n\n";

    config_file << "[population_1]\n";
    config_file << "name = Deggendorf\n";
    config_file << "size = 10\n";
    config_file << "vaccination_rate = 0.2\n";
    config_file << "patient_0 = true\n\n";

    config_file << "[population_2]\n";
    config_file << "name = Munich\n";
    config_file << "size = 20\n";
    config_file << "vaccination_rate = 0.5\n";
    config_file << "patient_0 = false\n";

    config_file.close();

    Simulation simulation(file_name);

    CHECK(simulation.load_config());

    CHECK(simulation.get_disease().get_name() == "Corona");
    CHECK(simulation.get_disease().get_duration() == 5);
    CHECK(simulation.get_disease().get_transmissibility() == doctest::Approx(0.1));

    CHECK(simulation.get_population_count() == 2);
    CHECK(simulation.count_vaccinated() == 12);
    CHECK(simulation.count_infectious() == 1);
    CHECK(simulation.count_susceptible() == 17);
    CHECK(simulation.count_recovered() == 0);
    CHECK(simulation.has_infectious());

    std::remove(file_name.c_str());
}

TEST_CASE("Simulation returns false when ini file is missing") {
    Simulation simulation("missing_file.ini");

    CHECK_FALSE(simulation.load_config());
}

TEST_CASE("Simulation can run after loading ini configuration") {
    const std::string file_name = "test_run_config.ini";

    std::ofstream config_file(file_name);

    config_file << "[global]\n";
    config_file << "simulation_name = test_run\n";
    config_file << "num_populations = 1\n";
    config_file << "simulation_runs = 1\n\n";

    config_file << "[disease]\n";
    config_file << "name = ShortDisease\n";
    config_file << "duration = 1\n";
    config_file << "transmissibility = 0.0\n\n";

    config_file << "[population_1]\n";
    config_file << "name = Deggendorf\n";
    config_file << "size = 10\n";
    config_file << "vaccination_rate = 0.0\n";
    config_file << "patient_0 = true\n";

    config_file.close();

    Simulation simulation(file_name);

    CHECK(simulation.load_config());

    simulation.run();

    CHECK(simulation.get_total_steps() == 1);
    CHECK(simulation.count_infectious() == 0);
    CHECK(simulation.count_recovered() == 1);
    CHECK(simulation.count_susceptible() == 9);

    std::remove(file_name.c_str());
}