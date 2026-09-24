#include "RoundRobinScheduler.h"
#include <algorithm>
#include <iostream>

RoundRobinScheduler::RoundRobinScheduler(int quantum)
    // initialize scheduler clock and RR configuration.
    // set process trackers to invalid state as CPU starts idle.
    : currentTime(0), timeQuantum(std::max(1, quantum)), currentProcessStartTime(-1),
      currentRunningProcessIndex(-1), currentQuantumSpent(0) {}

void RoundRobinScheduler::addProcess(const Process &p) {
  // append process to the internal process table.
  // it will be admitted to ready queue when arrival condition is met in tick.
  processes.push_back(p);
}

void RoundRobinScheduler::removeProcess(int pid) {
  auto it =
      std::find_if(processes.begin(), processes.end(), [pid](const Process &p) {
        return p.pid == pid;
      }); // The third argument is an unary function that takes one argument and
          // returns iterator on the first element that matches the condition.
          // Written as Lambda Expression.
  if (it != processes.end()) {
    int index =
        it - processes.begin(); // The index of the process to be deleted.
    if (index == currentRunningProcessIndex) {
      // Reset the trackers of the running process.
      currentProcessStartTime = -1;
      currentRunningProcessIndex = -1;
      currentQuantumSpent = 0;
    } else if (index < currentRunningProcessIndex) {
      // if an earlier element is removed, shift running index left to stay
      // valid.
      currentRunningProcessIndex--;
    }

    // remove process from the process table.
    processes.erase(it);

    // rebuild ready queue while fixing indices after erase.
    // skip the removed process if it was queued.
    std::vector<int> newReadyQueue;
    for (auto idx : readyQueue) {
      if (idx == index)
        continue;
      if (idx > index)
        // all indices after removed element shift by -1.
        newReadyQueue.push_back(idx - 1);
      else
        // indices before removed element remain unchanged.
        newReadyQueue.push_back(idx);
    }
    // commit updated ready queue.
    readyQueue = newReadyQueue;
  }
}

void RoundRobinScheduler::tick() {
  if (isFinished())
    return;

  for (size_t i = 0; i < processes.size(); i++) {
    if (!processes[i].hasStarted && !processes[i].isFinished &&
        processes[i].arrivalTime <= currentTime) {
      // if there is a process that hasn't started (Just Got Added) add it to
      // the ready queue and mark it as started.
      readyQueue.push_back(i);
      processes[i].hasStarted = true;
    }
  }

  // if the CPU is Idle while there are processes that are in the ready queue,
  // assign the first one to the CPU and remove it from the beginning. Will add
  // it to the back of the queue when it finishes its time quantum and wasn't
  // already finished.
  if (currentRunningProcessIndex == -1 && !readyQueue.empty()) {
    currentRunningProcessIndex = readyQueue.front();
    currentProcessStartTime = currentTime;
    currentQuantumSpent = 0;
    readyQueue.erase(readyQueue.begin());
  }

  // now for the logic of the current process running.
  if (currentRunningProcessIndex != -1) {
    Process &currentProcess = processes[currentRunningProcessIndex];
    // Advance time
    currentTime++;
    currentQuantumSpent++;
    currentProcess.remainingTime--;

    if (!ganttChart.empty() && ganttChart.back().pid == currentProcess.pid) {
      ganttChart.back().endTime = currentTime;
    } else {
      ganttChart.emplace_back(
          currentProcess.pid, currentTime - 1,
          currentTime); // If the gantt chart was empty or the last process that
                        // appeared on the chart wasn't the current process
                        // create another object of the execution record of the
                        // process that will take its part on the chart.
                        // "emplace_back" creates and object and pushes it into
                        // the queue
    }

    if (currentProcess.remainingTime == 0) {
      // mark the current process as finished and mark the CPU as idle so it can
      // get another process in the next tick.
      currentProcess.isFinished = true;
      currentProcess.completionTime = currentTime;
      currentProcess.turnaroundTime = currentTime - currentProcess.arrivalTime;
      currentProcess.waitingTime =
          currentProcess.turnaroundTime - currentProcess.burstTime;

      currentRunningProcessIndex = -1;
      currentProcessStartTime = -1;
      currentQuantumSpent = 0;
    } else if (currentQuantumSpent == timeQuantum) {
      // Check  if there was an added process at this same moment, if there is a
      // new process add it to the ready queue before adding the process that
      // was running to the back of the queue
      for (size_t i = 0; i < processes.size(); i++) {
        if (!processes[i].hasStarted && !processes[i].isFinished &&
            processes[i].arrivalTime <= currentTime) {
          // if there is a process that hasn't started (Just Got Added) add it
          // to the ready queue and mark it as started.
          readyQueue.push_back(i);
          processes[i].hasStarted = true;
        }
      }

      readyQueue.push_back(currentRunningProcessIndex);
      currentRunningProcessIndex = -1;
      currentProcessStartTime = -1;
      currentQuantumSpent = 0;
    }
  } else {
    currentTime++;
    // Add a slice to indicate that this is an idle time.
    if (!ganttChart.empty() && ganttChart.back().pid == -1) {
      ganttChart.back().endTime = currentTime;
    } else {
      ganttChart.emplace_back(
          -1, currentTime - 1,
          currentTime); // If the gantt chart was empty or the last process that
                        // appeared on the chart wasn't the current process
                        // create another object of the execution record of the
                        // process that will take its part on the chart.
                        // "emplace_back" creates and object and pushes it into
                        // the queue
    }
  }
}

void RoundRobinScheduler::runOffline() {
  // repeatedly call tick until all processes are completed.
  while (!isFinished()) {
    tick();
  }
}

bool RoundRobinScheduler::isFinished() const {
  // no work to do if process table is empty.
  if (processes.empty())
    return true;

  // if any process is not finished, scheduler is still active.
  for (auto p : processes) {
    if (!p.isFinished)
      return false;
  }
  // all processes finished.
  return true;
}

std::vector<Process> RoundRobinScheduler::getProcesses() const {
  // return a snapshot copy of current process states and metrics.
  return processes;
}

std::vector<ExecutionRecord> RoundRobinScheduler::getGanttChart() const {
  // return a snapshot copy of the executed timeline records.
  return ganttChart;
}

double RoundRobinScheduler::getAverageWaitingTime() const {
  // avoid division by zero when no processes exist.
  if (processes.empty())
    return 0.0;

  // aggregate waiting times then divide by number of processes.
  double totalWaitingTime = 0;
  int numOfProcesses = processes.size();
  for (auto p : processes) {
    totalWaitingTime += p.waitingTime;
  }
  return totalWaitingTime / numOfProcesses;
}

double RoundRobinScheduler::getAverageTurnaroundTime() const {
  // avoid division by zero when no processes exist.
  if (processes.empty())
    return 0.0;

  // aggregate turnaround times then divide by number of processes.
  double totalTurnaroundTime = 0;
  int numOfProcesses = processes.size();
  for (auto p : processes) {
    totalTurnaroundTime += p.turnaroundTime;
  }
  return totalTurnaroundTime / numOfProcesses;
}
