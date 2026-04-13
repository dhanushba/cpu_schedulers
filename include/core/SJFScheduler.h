#pragma once
#include "IScheduler.h"
#include <queue>
#include <vector>

class SJFScheduler : public IScheduler {
private:
  std::vector<Process> processes;
  std::vector<ExecutionRecord> ganttChart;

  struct SJFCompare {
    bool operator()(const Process &lhs, const Process &rhs) const {
      if (lhs.remainingTime == rhs.remainingTime)
        return lhs.pid > rhs.pid;
      return lhs.remainingTime > rhs.remainingTime;
    }
  };

  // ready queue for every second
  std::priority_queue<Process, std::vector<Process>, SJFCompare> priorityQ;

  int currentTime;
  bool isPreemptive;

  // Tracking variables
  int currentRunningProcessIndex;
  int currentProcessStartTime;

public:
  explicit SJFScheduler(bool preemptive);
  ~SJFScheduler() override = default;

  void addProcess(const Process &p) override;
  void removeProcess(int pid) override;
  void tick() override;
  void runOffline() override;
  bool isFinished() const override;

  std::vector<Process> getProcesses() const override;
  std::vector<ExecutionRecord> getGanttChart() const override;
  double getAverageWaitingTime() const override;
  double getAverageTurnaroundTime() const override;
  int getTopProcessIndex();
};
