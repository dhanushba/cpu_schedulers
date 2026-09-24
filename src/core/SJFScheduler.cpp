#include "SJFScheduler.h"
#include <algorithm>
#include <iostream>
using namespace std;

SJFScheduler::SJFScheduler(bool preemptive)
    : currentTime(0), isPreemptive(preemptive),
      currentRunningProcessIndex(-1), // idle CPU
      currentProcessStartTime(0) {}

void SJFScheduler::addProcess(const Process &p) { processes.push_back(p); }

void SJFScheduler::tick() {
  auto isRunnablePid = [this](int pid) -> bool {
    for (const auto &p : processes) {
      if (p.pid == pid)
        return !p.isFinished && p.remainingTime > 0;
    }
    return false; // pid was deleted
  };

  // for every new arrived process, push it to the ready queue
  for (int i = 0; i < processes.size(); i++) {
    if (!processes[i].isFinished && !processes[i].hasStarted &&
        processes[i].arrivalTime <= currentTime) {
      priorityQ.push(processes[i]); // ready processes
      processes[i].hasStarted = 1;
    }
  }

  while (!priorityQ.empty() && !isRunnablePid(priorityQ.top().pid)) {
    priorityQ.pop();
  }

  // if CPU is idle, run the top process in priority queue
  if (!priorityQ.empty() && currentRunningProcessIndex == -1) {
    currentRunningProcessIndex = getTopProcessIndex();
    priorityQ.pop();
    if (currentRunningProcessIndex == -1) {
      currentTime++;
      return;
    }

    if (processes[currentRunningProcessIndex].hasStarted == 0) {
      processes[currentRunningProcessIndex].hasStarted = 1;
    }
  }

  while (!priorityQ.empty() && !isRunnablePid(priorityQ.top().pid)) {
    priorityQ.pop();
  }

  // Preemptive Scheduling:
  // if CPU is NOT idle and the process hasn't finished yet:
  // Compare the current process's remaining time with the top of priority
  // queue's remaining time
  if (!priorityQ.empty() && currentRunningProcessIndex != -1 && isPreemptive) {
    if (priorityQ.top().remainingTime <
        processes[currentRunningProcessIndex].remainingTime) {
      // push the current process to wait again in the ready queue
      priorityQ.push(processes[currentRunningProcessIndex]);
      // run the one of the shortest remaining time now
      currentRunningProcessIndex = getTopProcessIndex();
      priorityQ.pop();

      if (processes[currentRunningProcessIndex].hasStarted == 0) {
        processes[currentRunningProcessIndex].hasStarted = 1;
      }
    }
    // if they are equal, let the running process continue
  }

  // Gantt Chart:
  if (currentRunningProcessIndex != -1) {
    // Check if it's the same process running before,
    if (!ganttChart.empty() &&
        ganttChart.back().pid == processes[currentRunningProcessIndex].pid)
      ganttChart.back().endTime++;
    else
      ganttChart.push_back(
          ExecutionRecord(processes[currentRunningProcessIndex].pid,
                          currentTime, currentTime + 1));

    // UPDATE REMAINING TIME FOR THIS PROCESS
    processes[currentRunningProcessIndex].remainingTime--;

    // IF IT'S THE LAST TIME FOR THIS PROCESS TO RUN
    if (processes[currentRunningProcessIndex].remainingTime <= 0) {
      // this process has finished //CPU is idle for the next tick
      processes[currentRunningProcessIndex].isFinished = 1;
      processes[currentRunningProcessIndex].completionTime =
          currentTime + 1; // the "end time" of the chart
      // Calculating turnaroundTime & waitingTime for this process
      processes[currentRunningProcessIndex].turnaroundTime =
          processes[currentRunningProcessIndex].completionTime -
          processes[currentRunningProcessIndex].arrivalTime;
      processes[currentRunningProcessIndex].waitingTime =
          processes[currentRunningProcessIndex].turnaroundTime -
          processes[currentRunningProcessIndex].burstTime;
      currentRunningProcessIndex = -1;
    }
  } else if (!ganttChart.empty() && ganttChart.back().pid == -1) {
    ganttChart.back().endTime++;
  } else {
    ganttChart.emplace_back(-1, currentTime, currentTime + 1);
  }

  currentTime++;
}

int SJFScheduler::getTopProcessIndex() {
  for (int i = 0; i < processes.size(); i++) {
    if (processes[i].pid == priorityQ.top().pid) {
      return i;
    }
  }
  return -1;
}

bool SJFScheduler::isFinished() const {
  if (processes.empty())
    return true;
  for (auto &p : processes) {
    if (!p.isFinished)
      return false;
  }
  return true;
}

void SJFScheduler::runOffline() {
  // tick is meant for this function
  // while all processes have not finished
  while (!isFinished())
    tick();
}

vector<Process> SJFScheduler::getProcesses() const { return processes; }

vector<ExecutionRecord> SJFScheduler::getGanttChart() const {
  return ganttChart;
}

double SJFScheduler::getAverageWaitingTime() const {
  if (processes.empty())
    return 0;
  double totalWaitingTime = 0;
  for (auto &p : processes) {
    totalWaitingTime += p.waitingTime;
  }

  return totalWaitingTime / processes.size();
}

double SJFScheduler::getAverageTurnaroundTime() const {
  if (processes.empty())
    return 0;
  double totalTurnaroundTime = 0;
  for (auto &p : processes) {
    totalTurnaroundTime += p.turnaroundTime;
  }

  return totalTurnaroundTime / processes.size();
}

void SJFScheduler::removeProcess(int pid) {
  for (int i = 0; i < processes.size(); i++) {
    if (processes[i].pid == pid) {
      if (currentRunningProcessIndex == i)
        currentRunningProcessIndex = -1;
      else if (currentRunningProcessIndex > i)
        currentRunningProcessIndex--; // shifting
      processes.erase(processes.begin() + i);
      // so we need to erase it from the ready queue, too
      std::priority_queue<Process, std::vector<Process>,
                          SJFScheduler::SJFCompare>
          priorityQ2;
      while (!priorityQ.empty()) {
        if (priorityQ.top().pid != pid) {
          priorityQ2.push(priorityQ.top());
        }
        priorityQ.pop();
      }
      priorityQ = priorityQ2; // update priority queue
      break;
    }
  }
}