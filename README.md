*This project has been created as part of the 42 curriculum by <your_login>.*

# Codexion

## Description

Codexion is a C simulation of the classic **Dining Philosophers** concurrency
problem, reskinned as coders sharing USB dongles in a circular co-working hub.

Each **coder** is represented by a POSIX thread that endlessly cycles through
three states: **compiling** (requires holding two dongles at once, one on
each side), **debugging**, and **refactoring**. Dongles are shared between
neighbouring coders, protected by mutexes, and become temporarily unusable
after being released (**cooldown**). A separate **monitor** thread watches
every coder and stops the simulation the instant one of them **burns out**
(fails to start compiling in time).

The project supports two arbitration policies for deciding who gets a
contested dongle:
- **FIFO**: requests are served in arrival order.
- **EDF** (Earliest Deadline First): the coder closest to burning out is
  served first.

The goal of the project is to practice thread synchronization (mutexes,
condition variables), deadlock avoidance, starvation prevention, and
building a custom priority queue (heap) from scratch.

## Instructions

### Compilation

```sh
make        # builds the "codexion" binary
make clean  # removes object files
make fclean # removes object files and the binary
make re     # fclean + all
```

The binary compiles with `-Wall -Wextra -Werror -pthread`.

### Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) |
| `time_to_burnout` | Max ms without starting a compile before burnout |
| `time_to_compile` | Duration of the compiling phase (ms) |
| `time_to_debug` | Duration of the debugging phase (ms) |
| `time_to_refactor` | Duration of the refactoring phase (ms) |
| `number_of_compiles_required` | Stop successfully once every coder reaches this count |
| `dongle_cooldown` | Ms a dongle stays unusable after being released |
| `scheduler` | `fifo` or `edf` |

Example:

```sh
./codexion 5 800 200 200 200 5 50 edf
```

### Example output

```
0 1 has taken a dongle
2 1 has taken a dongle
2 1 is compiling
202 1 is debugging
402 1 is refactoring
```

## Resources

- POSIX Threads Programming, LLNL Tutorial — https://hpc-tutorials.llnl.gov/posix/
- `man pthread_mutex_lock`, `man pthread_cond_timedwait`, `man clock_gettime`
- Dijkstra, E. W., *Hierarchical ordering of sequential processes* (origin of
  the Dining Philosophers problem and resource-ordering deadlock avoidance)
- Coffman, E. G. et al., *System Deadlocks* (the four Coffman conditions)
- Liu, C. L. and Layland, J., *Scheduling Algorithms for Multiprogramming in
  a Hard-Real-Time Environment* (origin of Earliest Deadline First scheduling)

### AI usage

AI (Claude) was used as a learning and pair-programming aid throughout this
project, in accordance with the school's AI usage guidelines:
- Explaining foundational concepts (threads, mutexes, condition variables,
  deadlock, starvation) from zero, since this was my first contact with
  concurrent programming.
- Helping design the module breakdown (parsing, dongle logic, scheduler,
  coder/monitor threads) before writing any code.
- Reviewing my own implementation after I pushed it, to identify concurrency
  bugs I could not see myself (see "Blocking cases handled" below for the
  bugs found and how they were fixed).
- All AI-suggested fixes were tested manually (varying number of coders,
  schedulers, and edge cases like a single coder) and I can explain the
  reasoning behind every one of them.

## Blocking cases handled

- **Deadlock prevention (Coffman's circular wait condition)**: coders
  acquire their two dongles in a fixed order (left then right), except the
  last coder (`id == number_of_coders`), who acquires right then left. This
  symmetry break makes a circular wait among all coders impossible, which
  removes one of the four necessary conditions for deadlock.
- **Starvation prevention**: dongle access is arbitrated by an explicit
  waiting queue per dongle (FIFO queue or EDF min-heap, built from scratch,
  no standard library priority queue used), so a coder can never be skipped
  indefinitely by a "luckier" thread waking up first.
- **Cooldown handling**: a dongle becomes usable again only `dongle_cooldown`
  ms after being released. Since cooldown expiry does not itself trigger any
  `pthread_cond_signal`, waiting threads use `pthread_cond_timedwait` to
  periodically re-check availability instead of `pthread_cond_wait`, which
  would otherwise sleep forever waiting for a signal that never comes.
- **Precise burnout detection**: a dedicated monitor thread polls every
  1 ms, well under the 10 ms tolerance required by the subject.
- **Log serialization**: all output goes through a single `log_state`
  function protected by a mutex, so two messages can never interleave on
  one line, and no further logs are emitted after the simulation has ended
  (except the burnout message itself).
- **Clean shutdown after burnout**: any coder thread still waiting on a
  dongle when the simulation ends is woken up (via the periodic
  `pthread_cond_timedwait`) and checks a shared `simulation_over` flag to
  abort its wait and return, instead of blocking forever — otherwise
  `pthread_join` in `main` would hang indefinitely.

## Thread synchronization mechanisms

- **`pthread_mutex_t` per dongle**: protects that dongle's `is_free` state
  and its waiting queue from concurrent access by multiple coder threads.
- **`pthread_mutex_t log_lock`**: serializes all `printf` calls.
- **`pthread_mutex_t end_lock`**: protects the shared `simulation_over` flag,
  written by the monitor thread and read by every coder thread.
- **`pthread_cond_t` per dongle**, used with `pthread_cond_timedwait`:
  coders wait on this condition variable while a dongle is unavailable
  (in use or in cooldown), and periodically re-check both the dongle state
  and whether the simulation has ended, avoiding both busy-waiting and the
  "silent cooldown expiry" deadlock described above.
- **Race-condition prevention example**: without the per-dongle mutex, two
  coders could both read `is_free == 1` at the same time and both proceed
  to take the same dongle. By requiring the mutex to be held for both the
  check and the state change (`dongle->is_free = 0`), this check-then-act
  sequence becomes atomic from the perspective of other threads.
- **Thread-safe coder ↔ monitor communication**: coders never message the
  monitor directly (the subject forbids coder-to-coder communication); the
  monitor instead reads each coder's `last_compile_start` and
  `compiles_done` fields directly. These are simple word-sized reads/writes
  without a dedicated mutex, which is safe in practice on the target
  architecture, while all *decisions* that affect program-wide control flow
  (`simulation_over`) go through `end_lock` to guarantee visibility across
  threads.
