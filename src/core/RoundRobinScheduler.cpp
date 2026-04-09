#include "RoundRobinScheduler.h"
#include <algorithm>
#include <iostream>

RoundRobinScheduler::RoundRobinScheduler(int quantum)
    : currentTime(0), timeQuantum(quantum) {}

void RoundRobinScheduler::addProcess(const Process &p) {
  processes.push_back(p);
}

void RoundRobinScheduler::tick() {}

bool RoundRobinScheduler::isFinished() const {
  if (processes.empty())
    return true;
  for (auto p : processes) {
    if (!p.isFinished)
      return false;
  }
  return true;
}

std::vector<Process> RoundRobinScheduler::getProcesses() const {
  return processes;
}

std::vector<ExecutionRecord> RoundRobinScheduler::getGanttChart() const {
  return ganttChart;
}

double RoundRobinScheduler::getAverageWaitingTime() const {
  int totalWaitingTime = 0;
  int numOfProcesses = processes.size();
  for (auto p : processes) {
    totalWaitingTime += p.waitingTime;
  }
  return totalWaitingTime / numOfProcesses;
}

double RoundRobinScheduler::getAverageTurnaroundTime() const {
  int totalTurnaroundTime = 0;
  int numOfProcesses = processes.size();
  for (auto p : processes) {
    totalTurnaroundTime += p.turnaroundTime;
  }
  return totalTurnaroundTime / numOfProcesses;
}