#ifndef HPC_DISEASE_SIMULATION_H_
#define HPC_DISEASE_SIMULATION_H_
#endif // HPC_DISEASE_SIMULATION_H_

#include <string>

class Person {
  public:
    enum class Status { SUSCEPTIBLE, INFECTIOUS, RECOVERED, VACCINATED };

    Person();
    bool is_infectious() const;
    bool is_susceptible() const;
    bool is_recovered() const;
    bool is_vaccinated() const;

  private:
    Status status;
    int days_remaining;
};

class Disease {
  public:
    Disease(int duration, double transmissibility);
    int getDuration() const;
    double getTransmissibility() const;
  private:
    int duration;
    double transmissibility;
};

class Population {};

class Simulation {
public:
  Simulation(std::string in_file = "disease_in.ini");
  void start();

private:
  std::string input_file;
};
