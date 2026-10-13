# Disease Simulation in C++

**An object-oriented disease spread simulator built with C++17, CMake, and automated tests.**

This project models how an infectious disease propagates through a population over time. It was developed as part of an HPC/QC programming lab and extended with configurable simulation parameters, experiment runners, and output data for investigating disease dynamics and herd immunity.

## Highlights

- **Modular C++ design:** Separate components for people, diseases, populations, and simulation behavior.
- **Configurable simulations:** Load experiment settings from configuration files.
- **Disease progression:** Simulate infection spread and track population-level outcomes over time.
- **Multiple runs and experiments:** Explore variation across runs and investigate herd immunity.
- **Results and reporting:** Generate simulation data and review the final experimental report.
- **Engineering workflow:** CMake builds, automated tests with doctest, and Docker support.

## Tech stack

| Area | Technologies |
| --- | --- |
| Language | C++17 |
| Build system | CMake |
| Testing | doctest, CTest |
| Environment | Linux, Docker |
| Version control | Git, GitLab, GitHub |

## Project structure

```text
.
├── CMakeLists.txt                         # Build targets and test registration
├── Dockerfile                             # Container configuration
├── Final_Disease_Simulation_Report.pdf    # Final report and experiments
├── README.md
├── configExample/                         # Example configuration files
├── examples/                              # Example inputs and/or outputs
├── include/                               # Shared headers and dependencies
├── report_data/                           # Recorded experiment data
├── scripts/                               # Supporting scripts
└── simulation/                            # Main C++ implementation and tests
```

The main CMake build defines a reusable `disease_core` library and two executables: `hpc_disease_simulation` for running the model and `all_tests` for automated tests.

## Build and run

### Requirements

- C++17-compatible compiler, such as GCC or Clang
- CMake **3.21 or later**
- Build tool such as Make

On Ubuntu, you can install the basic build tools with:

```bash
sudo apt update
sudo apt install build-essential cmake
```

### Compile

Clone this repository, then build it from its root:

```bash
git clone https://github.com/Shivatej-a11y/disease-simulation-cpp.git
cd disease-simulation-cpp
cmake -S . -B build
cmake --build build
```

### Run the simulation

```bash
./build/hpc_disease_simulation
```

The simulator has example configuration files under `configExample/`. See the source and example files for the specific configuration and run options supported by this version.

## Testing

The project includes automated tests built into the `all_tests` target and registered with CTest.

```bash
ctest --test-dir build --output-on-failure
```

Alternatively, run the test executable directly:

```bash
./build/all_tests
```

### Optional coverage report

Coverage generation requires `lcov` and `genhtml`, in addition to the normal compiler and build tools:

```bash
sudo apt install lcov
cmake -S . -B build-coverage -DENABLE_COVERAGE=ON
cmake --build build-coverage
cmake --build build-coverage --target coverage
```

The HTML report is generated under `build-coverage/coverage/index.html`.

## Experiments and results

The repository includes experiment data and a report covering the implementation and investigation of disease spread, including herd immunity scenarios.

- [Final Disease Simulation Report](Final_Disease_Simulation_Report.pdf)
- [Recorded experiment data](report_data/)
- [Example configurations](configExample/)

> **Scope:** This is an educational simulation model intended for software engineering and computational experimentation, not for clinical or public-health prediction.

## What I practiced

- Designing and implementing modular object-oriented C++ software
- Separating core simulation logic from executable entry points
- Writing and running automated tests
- Managing builds with CMake and developing in Linux
- Configuring repeatable simulation experiments and interpreting their outputs
- Working with Git branches, commits, and a structured development workflow

## Project context

Developed for the **HPC/QC Programming Lab** at **Deggendorf Institute of Technology (TH Deggendorf)**. This GitHub repository contains the standalone Disease Simulation component, extracted from the university's larger lab repository with its relevant Git commit history.
