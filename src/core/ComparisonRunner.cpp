#include "ComparisonRunner.h"

#include "FCFSScheduler.h"
#include "IScheduler.h"
#include "PriorityScheduler.h"
#include "RoundRobinScheduler.h"
#include "SJFScheduler.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

namespace {

struct SchedulerConfiguration {
  SchedulerMode mode;
  const char *name;
};

constexpr SchedulerConfiguration configurations[] = {
    {SchedulerMode::FCFS, "FCFS"},
    {SchedulerMode::SJFNonPreemptive, "SJF (Non-Preemptive)"},
    {SchedulerMode::SJFPreemptive, "SJF (Preemptive)"},
    {SchedulerMode::PriorityNonPreemptive, "Priority (Non-Preemptive)"},
    {SchedulerMode::PriorityPreemptive, "Priority (Preemptive)"},
    {SchedulerMode::RoundRobin, "Round Robin"},
};

std::unique_ptr<IScheduler> createScheduler(SchedulerMode mode, int quantum) {
  switch (mode) {
  case SchedulerMode::FCFS:
    return std::make_unique<FCFSScheduler>();
  case SchedulerMode::SJFNonPreemptive:
    return std::make_unique<SJFScheduler>(false);
  case SchedulerMode::SJFPreemptive:
    return std::make_unique<SJFScheduler>(true);
  case SchedulerMode::PriorityNonPreemptive:
    return std::make_unique<PriorityScheduler>(false);
  case SchedulerMode::PriorityPreemptive:
    return std::make_unique<PriorityScheduler>(true);
  case SchedulerMode::RoundRobin:
    return std::make_unique<RoundRobinScheduler>(quantum);
  }

  throw std::invalid_argument("Unsupported scheduler mode");
}

double calculateCpuUtilization(const std::vector<ExecutionRecord> &timeline) {
  int busyTime = 0;
  int makespan = 0;

  for (const auto &record : timeline) {
    const int duration = std::max(0, record.endTime - record.startTime);
    makespan = std::max(makespan, record.endTime);
    if (record.pid >= 0)
      busyTime += duration;
  }

  return makespan > 0 ? (100.0 * busyTime) / makespan : 0.0;
}

int countContextSwitches(const std::vector<ExecutionRecord> &timeline) {
  int switches = 0;
  int previousPid = -1;

  for (const auto &record : timeline) {
    if (record.pid < 0)
      continue;
    if (previousPid >= 0 && previousPid != record.pid)
      switches++;
    previousPid = record.pid;
  }

  return switches;
}

ComparisonResult runConfiguration(const SchedulerConfiguration &configuration,
                                  const std::vector<Process> &workload,
                                  int quantum) {
  auto scheduler = createScheduler(configuration.mode, quantum);
  for (const auto &process : workload) {
    scheduler->addProcess(Process(process.pid, process.arrivalTime,
                                  process.burstTime, process.priority));
  }

  scheduler->runOffline();
  auto timeline = scheduler->getGanttChart();

  return {configuration.mode,
          configuration.name,
          scheduler->getProcesses(),
          timeline,
          scheduler->getAverageWaitingTime(),
          scheduler->getAverageTurnaroundTime(),
          calculateCpuUtilization(timeline),
          countContextSwitches(timeline)};
}

} // namespace

std::vector<ComparisonResult>
ComparisonRunner::run(const std::vector<Process> &workload,
                      int roundRobinQuantum) {
  if (roundRobinQuantum <= 0)
    throw std::invalid_argument("Round Robin quantum must be positive");

  std::vector<ComparisonResult> results;
  results.reserve(std::size(configurations));
  for (const auto &configuration : configurations) {
    results.push_back(
        runConfiguration(configuration, workload, roundRobinQuantum));
  }
  return results;
}