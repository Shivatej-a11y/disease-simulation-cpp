#!/usr/bin/env python3

import argparse
import csv
import shutil
import subprocess
from pathlib import Path

import matplotlib.pyplot as plt


def write_config(file_path, duration, transmissibility, vaccination_rate, simulation_runs):
    file_path.write_text(
        f"""; Config file for a disease simulation

[global]
simulation_name = herd_immunity_experiment
num_populations = 1
simulation_runs = {simulation_runs}

[disease]
name = ProfessorDisease
duration = {duration}
transmissibility = {transmissibility}

[population_1]
name = ExperimentPopulation
size = 30000
vaccination_rate = {vaccination_rate}
patient_0 = true
"""
    )


def read_stats(file_path):
    values = {}

    with file_path.open(newline="") as file:
        reader = csv.DictReader(file)

        for row in reader:
            values[row["key"]] = {
                "mean": float(row["mean"]),
                "std": float(row["std"]),
            }

    return values


def read_details(file_path):
    rows = []

    with file_path.open(newline="") as file:
        reader = csv.DictReader(file)
        step_counter = {}

        for row in reader:
            key = (row["name"], row["run"])
            step = step_counter.get(key, 0)
            step_counter[key] = step + 1

            rows.append({
                "step": step,
                "name": row["name"],
                "run": int(row["run"]),
                "infectious": int(row["infectious"]),
                "recovered": int(row["recovered"]),
                "susceptible": int(row["susceptiple"]),
                "vaccinated": int(row["vaccinated"]),
            })

    return rows


def write_summary_csv(file_path, summary_rows):
    with file_path.open("w", newline="") as file:
        writer = csv.DictWriter(
            file,
            fieldnames=[
                "vaccination_rate",
                "recovered_mean",
                "recovered_std",
                "total_steps_mean",
                "total_steps_std",
            ],
        )

        writer.writeheader()
        writer.writerows(summary_rows)


def plot_recovered_vs_vaccination(summary_rows, output_file):
    vaccination_rates = [row["vaccination_rate"] * 100 for row in summary_rows]
    recovered_values = [row["recovered_mean"] for row in summary_rows]

    plt.figure()
    plt.scatter(vaccination_rates, recovered_values)
    plt.xlabel("vaccination_rate")
    plt.ylabel("recovered")
    plt.title("Recovered people vs vaccination rate")
    plt.savefig(output_file, dpi=200, bbox_inches="tight")
    plt.close()


def plot_timestep_details(details_rows, output_file):
    selected_rows = [row for row in details_rows if row["run"] == 0]

    steps = [row["step"] for row in selected_rows]
    infectious = [row["infectious"] for row in selected_rows]
    recovered = [row["recovered"] for row in selected_rows]
    susceptible = [row["susceptible"] for row in selected_rows]
    vaccinated = [row["vaccinated"] for row in selected_rows]

    plt.figure()
    plt.plot(steps, infectious, label="infectious")
    plt.plot(steps, recovered, label="recovered")
    plt.plot(steps, susceptible, label="susceptiple")
    plt.plot(steps, vaccinated, label="vaccinated")
    plt.xlabel("steps")
    plt.ylabel("counts")
    plt.title("Population states over time")
    plt.legend()
    plt.savefig(output_file, dpi=200, bbox_inches="tight")
    plt.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", default="./build/hpc_disease_simulation")
    parser.add_argument("--duration", type=int, required=True)
    parser.add_argument("--transmissibility", type=float, required=True)
    parser.add_argument("--simulation-runs", type=int, default=5)
    parser.add_argument("--single-rate", type=float, default=0.1)
    parser.add_argument("--out", default="report_data")

    args = parser.parse_args()

    output_dir = Path(args.out)
    output_dir.mkdir(parents=True, exist_ok=True)

    binary = Path(args.binary)

    if not binary.exists():
        raise FileNotFoundError(f"Simulation binary not found: {binary}")

    summary_rows = []
    single_rate_details = None

    for rate_percent in range(0, 101, 10):
        vaccination_rate = rate_percent / 100.0
        run_dir = output_dir / f"vaccination_{rate_percent:03d}"

        if run_dir.exists():
            shutil.rmtree(run_dir)

        run_dir.mkdir(parents=True, exist_ok=True)

        config_file = run_dir / "disease_in.ini"

        write_config(
            config_file,
            args.duration,
            args.transmissibility,
            vaccination_rate,
            args.simulation_runs,
        )

        subprocess.run([str(binary), str(config_file)], check=True)

        stats_file = run_dir / "disease_stats.csv"
        details_file = run_dir / "disease_details.csv"

        stats = read_stats(stats_file)

        summary_rows.append({
            "vaccination_rate": vaccination_rate,
            "recovered_mean": stats["recovered_persons"]["mean"],
            "recovered_std": stats["recovered_persons"]["std"],
            "total_steps_mean": stats["total_steps"]["mean"],
            "total_steps_std": stats["total_steps"]["std"],
        })

        if abs(vaccination_rate - args.single_rate) < 0.000001:
            single_rate_details = read_details(details_file)

    write_summary_csv(output_dir / "herd_summary.csv", summary_rows)

    plot_recovered_vs_vaccination(
        summary_rows,
        output_dir / "plot_recovered_vs_vaccination_rate.png",
    )

    if single_rate_details is not None:
        plot_timestep_details(
            single_rate_details,
            output_dir / "plot_timestep_single_rate.png",
        )

    print(f"Experiment data written to {output_dir}")


if __name__ == "__main__":
    main()
