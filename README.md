# CPU Scheduler Simulator

This is a university project that simulates common CPU scheduling algorithms.

## What it does

- Lets you add processes (arrival time, burst time, priority)
- Runs different scheduling algorithms
- Shows process status in a table and execution order over time
- Displays simple performance metrics (waiting and turnaround averages)

## Algorithms included

- FCFS
- SJF (preemptive and non-preemptive)
- Priority (preemptive and non-preemptive)
- Round Robin

## Tech stack

- C++17
- Qt6 Widgets
- CMake

## Build and run (Linux/macOS)

```bash
cmake -S . -B build
cmake --build build
./build/CPUScheduler
```

## Build and run (Windows)

Use CMake with a Qt6 kit installed, then run the generated `CPUScheduler.exe`.

If you want a portable package, this repo includes a GitHub Actions workflow that builds and uploads a Windows release `.zip` on version tags like `v1.0.0`.

## Notes

- This project was built for learning purposes.
- Focus was on scheduling logic and simulation behavior.
