
# Dining Philosophers — philo (Mandatory + Bonus)

This repository contains an implementation of the Dining Philosophers problem in C. It includes two variants:

- `philodophers_mandatory`: the mandatory part implementing the core simulation.
- `philodophers_bonus`: the bonus part (processes/advanced monitoring variations).

**Goal:** simulate philosophers sitting at a table alternating between thinking, eating and sleeping while avoiding deadlocks and ensuring correct timing and synchronization.

**Highlights:**
- Written in C using POSIX threads and synchronization primitives.
- Buildable with the provided `Makefile`s in each subdirectory.
- Includes argument parsing and runtime monitoring.

**Requirements:**
- A Unix-like OS (Linux recommended).
- A C compiler (`cc`/`gcc`) and `make`.
- POSIX threads support (`-pthread`).

**Quick Start**

Build the mandatory part:

```sh
cd philodophers_mandatory
make
```

Build the bonus part:

```sh
cd philodophers_bonus
make
```

After building you'll have the executables `philo` (mandatory) and `philo_bonus` (bonus) in their respective folders.

**Usage**

Run the program with the following arguments:

```sh
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

- `number_of_philosophers`: total philosophers (and forks) around the table
- `time_to_die` (ms): time a philosopher may go without eating before dying
- `time_to_eat` (ms): how long a philosopher spends eating
- `time_to_sleep` (ms): how long a philosopher spends sleeping
- `number_of_times_each_philosopher_must_eat` (optional): if provided, simulation stops when each philosopher has eaten this many times

Example (mandatory):

```sh
cd philodophers_mandatory
./philo 5 800 200 200
```

Example (bonus):

```sh
cd philodophers_bonus
./philo_bonus 4 410 200 200 7
```

**Behavior / Notes**
- The simulation prints timestamped actions for each philosopher (taking forks, eating, sleeping, thinking, dying).
- The implementation attempts to avoid deadlock and starvation through ordering, monitoring, and proper locking.
- Time values are handled in milliseconds; the program uses usleep and gettimeofday (or equivalent) to measure and sleep accurately.

**Mandatory: threads & mutexes**
- The mandatory variant (`philo`) uses POSIX threads (`pthread`) and mutexes to coordinate philosophers.
- Synchronization primitives used in the code (see `philodophers_mandatory/philo.h` and `initialization.c`):
	- An array of mutexes allocated as `forks` — one mutex per fork.
	- `mutex_data` — protects printing to stdout/stderr so action lines are not interleaved.
	- `mutex_monitor` — protects monitor-related shared fields (timestamps, meal counters).
	- `mutex_dead` — protects the `deads` flag when a philosopher dies.
- Threads are created with `pthread_create` (one per philosopher) and joined with `pthread_join`; a central monitor routine (`__monitor__`) checks timeouts and stops the simulation on death or when all meals are completed.
- Special cases and behavior:
	- The single-philosopher case is handled so the program doesn't deadlock when there is only one fork available.
	- Timestamps are printed relative to a `start` time (milliseconds) and colorized output is used for readability.
- Cleanup: mutexes are destroyed with `pthread_mutex_destroy` in `destroy_mutex`, and allocated arrays are freed on exit.
- Build and debug tips: compile with `-pthread` (already present in the `Makefile`). To help find data races or threading issues, try adding `-g -fsanitize=thread` to `CFLAGS` (if your toolchain supports ThreadSanitizer), or run with `valgrind --tool=helgrind` / `helgrind` for thread-related issues (note these tools can affect timing-sensitive behavior).

**Bonus: processes & semaphores**
- The bonus variant (`philo_bonus`) uses processes (via `fork`) instead of threads and coordinates them with POSIX named semaphores.
- Semaphore names used in the implementation: `"forks"`, `"write"`, `"eat"`, `"monitor"`, and `"dead"` — each providing synchronization for forks, output, meal tracking and monitoring respectively.
- The program opens these semaphores at startup and closes/unlinks them on exit (`sem_close` + `sem_unlink`). If a previous run crashed and left named semaphores in the kernel, you may need to `sem_unlink` them (or reboot) before rerunning.
- Because the bonus uses processes, debugging and race-detection are different than the threaded version — any shared state is managed explicitly through semaphores and shared memory constructs where needed.

**Testing & Debugging Tips**
- Run the programs with small numbers and short timings to validate behavior quickly.
- For race detection and undefined behavior, consider compiling with sanitizers (adjust `CFLAGS` in `Makefile`):

```sh
