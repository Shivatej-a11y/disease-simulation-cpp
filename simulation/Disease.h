#ifndef HPC_DISEASE_DISEASE_H_
#define HPC_DISEASE_DISEASE_H_

#include <string>

class Disease {
public:
    Disease();
    Disease(const std::string& name, int duration, double transmissibility);

    std::string get_name() const;
    int get_duration() const;
    double get_transmissibility() const;

private:
    std::string name;
    int duration;
    double transmissibility;
};

#endif // HPC_DISEASE_PERSON_H