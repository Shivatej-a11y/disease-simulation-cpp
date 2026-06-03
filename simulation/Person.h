#ifndef HPC_DISEASE_PERSON_H_
#define HPC_DISEASE_PERSON_H_

class Person {
public:
    enum class Status {
        SUSCEPTIBLE,
        INFECTIOUS,
        RECOVERED,
        VACCINATED
    };

    Person();

    bool is_susceptible() const;
    bool is_infectious() const;
    bool is_recovered() const;
    bool is_vaccinated() const;

    void infect();
    void vaccinate();
    void recover();
    void progress_one_day(int disease_duration);

    Status get_status() const;
    int get_days_infected() const;

private:
    Status status;
    int days_infected;
};

#endif // HPC_DISEASE_PERSON_H