#include "ComparisonRunner.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

int failures = 0;

void expect(bool condition, const char *description) {
  if (condition)
    return;
  std::cerr << "FAIL: " << description << '\n';
  failures++;
}

void expectNear(double actual, double expected, const char *description) {
  if (std::abs(actual - expected) < 0.0001)
    return;
  std::cerr << "FAIL: " << description << " (expected " << expected
            << ", got " << actual << ")\n";
  failures++;
}

const ComparisonResult &findResult(const std::vector<ComparisonResult> &results,
                                   SchedulerMode mode) {
  for (const auto &result : results) {
    if (result.mode == mode)
      return result;
  }
  throw std::runtime_error("Comparison result was not found");
}

void testRunsAllModesWithoutMutatingWorkload() {
  std::vector<Process> workload = {Process(1, 0, 5, 2),
                                   Process(2, 1, 3, 1)};
  const auto results = ComparisonRunner::run(workload, 2);

  expect(results.size() == 6, "comparison returns all six modes");
  expect(workload[0].remainingTime == 5 && !workload[0].isFinished,
         "comparison does not mutate input workload");

  for (const auto &result : results) {
    expect(result.processes.size() == workload.size(),
           "each mode returns every process");
    for (const auto &process : result.processes)
      expect(process.isFinished, "each comparison process is finished");
    expectNear(result.cpuUtilization, 100.0,
               "continuous workload has full CPU utilization");
  }

  const auto &fcfs = findResult(results, SchedulerMode::FCFS);
  expectNear(fcfs.averageWaitingTime, 2.0, "FCFS average waiting time");
  expectNear(fcfs.averageTurnaroundTime, 6.0,
             "FCFS average turnaround time");
  expect(fcfs.contextSwitches == 1, "FCFS context switch count");

  const auto &roundRobin = findResult(results, SchedulerMode::RoundRobin);
  expectNear(roundRobin.averageWaitingTime, 3.0,
             "Round Robin average waiting time");
  expectNear(roundRobin.averageTurnaroundTime, 7.0,
             "Round Robin average turnaround time");
  expect(roundRobin.contextSwitches == 4,
         "Round Robin context switch count");
}

void testUtilizationIncludesInitialIdleTime() {
  const auto results = ComparisonRunner::run({Process(1, 2, 2)}, 2);
  for (const auto &result : results)
    expectNear(result.cpuUtilization, 50.0,
               "utilization includes time before first arrival");
}

void testDisplayedTwoProcessComparison() {
  const auto results =
      ComparisonRunner::run({Process(1, 0, 3, 1), Process(2, 0, 5, 1)}, 1);

  for (const auto &result : results) {
    expectNear(result.cpuUtilization, 100.0,
               "displayed workload has full CPU utilization");
    if (result.mode == SchedulerMode::RoundRobin)
      continue;
    expectNear(result.averageWaitingTime, 1.5,
               "non-RR displayed average waiting time");
    expectNear(result.averageTurnaroundTime, 5.5,
               "non-RR displayed average turnaround time");
    expect(result.contextSwitches == 1,
           "non-RR displayed context switch count");
  }

  const auto &roundRobin = findResult(results, SchedulerMode::RoundRobin);
  expectNear(roundRobin.averageWaitingTime, 2.5,
             "displayed Round Robin average waiting time");
  expectNear(roundRobin.averageTurnaroundTime, 6.5,
             "displayed Round Robin average turnaround time");
  expect(roundRobin.contextSwitches == 5,
         "displayed Round Robin context switch count");
}

void testRejectsInvalidQuantum() {
  bool threw = false;
  try {
    ComparisonRunner::run({}, 0);
  } catch (const std::invalid_argument &) {
    threw = true;
  }
  expect(threw, "comparison rejects a non-positive Round Robin quantum");
}

} // namespace

int main() {
  testRunsAllModesWithoutMutatingWorkload();
  testUtilizationIncludesInitialIdleTime();
  testDisplayedTwoProcessComparison();
  testRejectsInvalidQuantum();

  if (failures != 0) {
    std::cerr << failures << " comparison assertion(s) failed.\n";
    return 1;
  }

  std::cout << "All comparison runner tests passed.\n";
  return 0;
}