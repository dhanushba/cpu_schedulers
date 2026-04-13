#pragma once
#include "IScheduler.h"
#include <functional>
#include <queue>
#include <vector>

class PriorityScheduler : public IScheduler {
private:
  std::vector<Process> processes;
  std::vector<ExecutionRecord> ganttChart;
  int currentTime;
  bool isPreemptive;

  struct PriorityCompare {
    bool operator()(const Process &lhs, const Process &rhs) const {
      if (lhs.priority == rhs.priority)
        return lhs.pid > rhs.pid;
      return lhs.priority > rhs.priority;
    }
  };

  // definition to let process work with minimum heap needed in the
  // implementation
  std::priority_queue<Process, std::vector<Process>, PriorityCompare>
      priorityQueue;

  // Tracking variables
  int currentRunningProcessIndex;
  int currentProcessStartTime;

public:
  explicit PriorityScheduler(bool preemptive);
  ~PriorityScheduler() override = default;

  void addProcess(const Process &p) override;
  void removeProcess(int pid) override;
  void tick() override;
  void runOffline() override;
  bool isFinished() const override;

  std::vector<Process> getProcesses() const override;
  std::vector<ExecutionRecord> getGanttChart() const override;
  double getAverageWaitingTime() const override;
  double getAverageTurnaroundTime() const override;
};
