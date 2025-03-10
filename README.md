# Philosopher Project

## Overview

The **Philosopher** project at 42 is a multi-threading and synchronization exercise in C. In this project, you will implement a simulation of philosophers sitting around a table, thinking, eating, and sharing a common resource: forks. The goal of the project is to gain a deep understanding of concurrency, synchronization, and deadlock prevention using POSIX threads and mutexes.

The problem is inspired by the famous "Dining Philosophers" problem, where each philosopher needs two forks to eat, but forks are shared between them. The challenge is to ensure that all philosophers can eat without encountering deadlocks or resource starvation.

## Requirements

- **Unix-based Operating System** (Linux, macOS, etc.)
- **C Compiler** (gcc or clang)
- **Make Utility** for building the project
- **POSIX Threads** for multi-threading

## Installation

1. Clone the repository:

    ...

2. Navigate to the project directory:

    ```bash
    cd philosopher
    ```

3. Build the project using `make`:

    ```bash
    make
    ```

4. Run the program:

    ```bash
    ./philosopher [number_of_philosophers] [time_to_die] [time_to_eat] [time_to_sleep] [number_of_times_each_philosopher_must_eat]
    ```

    - `number_of_philosophers`: The number of philosophers in the simulation.
    - `time_to_die`: Time in milliseconds before a philosopher dies from starvation.
    - `time_to_eat`: Time in milliseconds it takes for a philosopher to eat.
    - `time_to_sleep`: Time in milliseconds a philosopher sleeps between meals.
    - `number_of_times_each_philosopher_must_eat`: The number of times each philosopher must eat before the simulation ends (optional).

## Simulation Process

- Each philosopher alternates between thinking and eating.
- To eat, a philosopher needs two forks, which are shared with their neighbors.
- Philosophers should never starve, meaning they must get a chance to eat, and the simulation should prevent deadlock, where no philosopher can make progress.
- The program will print each philosopher's actions (eating, thinking, sleeping) to the terminal, and handle timeouts where philosophers "die" from starvation.

### Example Usage:

```bash
./philosopher 5 800 200 200 3
