FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app/part_1_disease

RUN cmake -S /app/part_1_disease -B /app/build \
    && cmake --build /app/build --target hpc_disease_simulation

WORKDIR /scratch

ENTRYPOINT ["/app/build/hpc_disease_simulation", "/scratch/disease_in.ini"]