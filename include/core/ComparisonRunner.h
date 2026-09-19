#pragma once

#include "ExecutionRecord.h"
#include "Process.h"

#include <string>
#include <vector>

enum class SchedulerMode {
  FCFS,
  SJFNonPreemptive,
  SJFPreemptive,
  PriorityNonPreemptive,
  PriorityPreemptive,
  RoundRobin,
};

struct ComparisonResult {
  SchedulerMode mode;
  std::string name;
  std::vector<Process> processes;
  std::vector<ExecutionRecord> timeline;
  double averageWaitingTime;
  double averageTurnaroundTime;
  double cpuUtilization;
  int contextSwitches;
};

class ComparisonRunner {
public:
  static std::vector<ComparisonResult>
  run(const std::vector<Process> &workload, int roundRobinQuantum = 2);
};