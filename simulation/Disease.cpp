#include "Disease.h"

Disease::Disease()
    : name("unknown"), duration(0), transmissibility(0.0) {}

Disease::Disease(const std::string& name, int duration, double transmissibility)
    : name(name), duration(duration), transmissibility(transmissibility) {}

std::string Disease::get_name() const {
    return name;
}

int Disease::get_duration() const {
    return duration;
}

double Disease::get_transmissibility() const {
    return transmissibility;
}
