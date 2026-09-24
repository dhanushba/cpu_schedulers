#include "FCFSScheduler.h"
#include "PriorityScheduler.h"
#include "RoundRobinScheduler.h"
#include "SJFScheduler.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void expectEqual(int actual, int expected, const std::string &description) {
  if (actual == expected)
    return;

  std::cerr << "FAIL: " << description << " (expected " << expected
            << ", got " << actual << ")\n";
  failures++;
}

void expectNear(double actual, double expected,
                const std::string &description) {
  if (std::abs(actual - expected) < 0.0001)
    return;

  std::cerr << "FAIL: " << description << " (expected " << expected
            << ", got " << actual << ")\n";
  failures++;
}

const Process &processByPid(const std::vector<Process> &processes, int pid) {
  for (const auto &process : processes) {
    if (process.pid == pid)
      return process;
  }

  std::cerr << "FAIL: process P" << pid << " was not found\n";
  failures++;
  return processes.front();
}

void expectTimeline(const std::vector<ExecutionRecord> &actual,
                    const std::vector<ExecutionRecord> &expected,
                    const std::string &description) {
  expectEqual(static_cast<int>(actual.size()), static_cast<int>(expected.size()),
              description + " record count");
  if (actual.size() != expected.size())
    return;

  for (size_t index = 0; index < actual.size(); index++) {
    expectEqual(actual[index].pid, expected[index].pid,
                description + " PID at record " + std::to_string(index));
    expectEqual(actual[index].startTime, expected[index].startTime,
                description + " start at record " + std::to_string(index));
    expectEqual(actual[index].endTime, expected[index].endTime,
                description + " end at record " + std::to_string(index));
  }
}

void testFcfs() {
  FCFSScheduler scheduler;
  scheduler.addProcess(Process(1, 0, 5));
  scheduler.addProcess(Process(2, 1, 3));
  scheduler.runOffline();

  const auto processes = scheduler.getProcesses();
  const auto &p1 = processByPid(processes, 1);
  const auto &p2 = processByPid(processes, 2);
  expectEqual(p1.waitingTime, 0, "FCFS P1 waiting time");
  expectEqual(p1.turnaroundTime, 5, "FCFS P1 turnaround time");
  expectEqual(p2.waitingTime, 4, "FCFS P2 waiting time");
  expectEqual(p2.turnaroundTime, 7, "FCFS P2 turnaround time");
  expectNear(scheduler.getAverageWaitingTime(), 2.0,
             "FCFS average waiting time");
  expectNear(scheduler.getAverageTurnaroundTime(), 6.0,
             "FCFS average turnaround time");
  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 5), ExecutionRecord(2, 5, 8)},
                 "FCFS timeline");
}

void testSjfNonPreemptive() {
  SJFScheduler scheduler(false);
  scheduler.addProcess(Process(1, 0, 7));
  scheduler.addProcess(Process(2, 2, 4));
  scheduler.addProcess(Process(3, 4, 1));
  scheduler.runOffline();

  const auto processes = scheduler.getProcesses();
  expectEqual(processByPid(processes, 1).waitingTime, 0,
              "SJF non-preemptive P1 waiting time");
  expectEqual(processByPid(processes, 2).waitingTime, 6,
              "SJF non-preemptive P2 waiting time");
  expectEqual(processByPid(processes, 3).waitingTime, 3,
              "SJF non-preemptive P3 waiting time");
  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 7), ExecutionRecord(3, 7, 8),
                  ExecutionRecord(2, 8, 12)},
                 "SJF non-preemptive timeline");
}

void testSjfPreemptive() {
  SJFScheduler scheduler(true);
  scheduler.addProcess(Process(1, 0, 7));
  scheduler.addProcess(Process(2, 2, 4));
  scheduler.addProcess(Process(3, 4, 1));
  scheduler.runOffline();

  const auto processes = scheduler.getProcesses();
  expectEqual(processByPid(processes, 1).waitingTime, 5,
              "SJF preemptive P1 waiting time");
  expectEqual(processByPid(processes, 2).waitingTime, 1,
              "SJF preemptive P2 waiting time");
  expectEqual(processByPid(processes, 3).waitingTime, 0,
              "SJF preemptive P3 waiting time");
  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 2), ExecutionRecord(2, 2, 4),
                  ExecutionRecord(3, 4, 5), ExecutionRecord(2, 5, 7),
                  ExecutionRecord(1, 7, 12)},
                 "SJF preemptive timeline");
}

void testPriorityNonPreemptive() {
  PriorityScheduler scheduler(false);
  scheduler.addProcess(Process(1, 0, 5, 2));
  scheduler.addProcess(Process(2, 1, 2, 1));
  scheduler.addProcess(Process(3, 2, 1, 3));
  scheduler.runOffline();

  const auto processes = scheduler.getProcesses();
  expectEqual(processByPid(processes, 1).waitingTime, 0,
              "Priority non-preemptive P1 waiting time");
  expectEqual(processByPid(processes, 2).waitingTime, 4,
              "Priority non-preemptive P2 waiting time");
  expectEqual(processByPid(processes, 3).waitingTime, 5,
              "Priority non-preemptive P3 waiting time");
  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 5), ExecutionRecord(2, 5, 7),
                  ExecutionRecord(3, 7, 8)},
                 "Priority non-preemptive timeline");
}

void testPriorityPreemptive() {
  PriorityScheduler scheduler(true);
  scheduler.addProcess(Process(1, 0, 5, 2));
  scheduler.addProcess(Process(2, 1, 2, 1));
  scheduler.addProcess(Process(3, 2, 1, 3));
  scheduler.runOffline();

  const auto processes = scheduler.getProcesses();
  expectEqual(processByPid(processes, 1).waitingTime, 2,
              "Priority preemptive P1 waiting time");
  expectEqual(processByPid(processes, 2).waitingTime, 0,
              "Priority preemptive P2 waiting time");
  expectEqual(processByPid(processes, 3).waitingTime, 5,
              "Priority preemptive P3 waiting time");
  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 1), ExecutionRecord(2, 1, 3),
                  ExecutionRecord(1, 3, 7), ExecutionRecord(3, 7, 8)},
                 "Priority preemptive timeline");
}

void testRoundRobin() {
  RoundRobinScheduler scheduler(2);
  scheduler.addProcess(Process(1, 0, 5));
  scheduler.addProcess(Process(2, 1, 3));
  scheduler.runOffline();

  const auto processes = scheduler.getProcesses();
  expectEqual(processByPid(processes, 1).waitingTime, 3,
              "Round Robin P1 waiting time");
  expectEqual(processByPid(processes, 2).waitingTime, 3,
              "Round Robin P2 waiting time");
  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 2), ExecutionRecord(2, 2, 4),
                  ExecutionRecord(1, 4, 6), ExecutionRecord(2, 6, 7),
                  ExecutionRecord(1, 7, 8)},
                 "Round Robin timeline");
}

void testSjfEqualBurstUsesArrivalOrder() {
  SJFScheduler scheduler(false);
  scheduler.addProcess(Process(3, 0, 5));
  scheduler.addProcess(Process(1, 4, 2));
  scheduler.addProcess(Process(2, 1, 2));
  scheduler.runOffline();

  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(3, 0, 5), ExecutionRecord(2, 5, 7),
                  ExecutionRecord(1, 7, 9)},
                 "SJF equal-burst arrival-order timeline");
}

void testPriorityTieUsesArrivalOrder() {
  PriorityScheduler scheduler(false);
  scheduler.addProcess(Process(3, 0, 5, 0));
  scheduler.addProcess(Process(1, 4, 1, 2));
  scheduler.addProcess(Process(2, 1, 1, 2));
  scheduler.runOffline();

  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(3, 0, 5), ExecutionRecord(2, 5, 6),
                  ExecutionRecord(1, 6, 7)},
                 "Priority equal-priority arrival-order timeline");
}

void testSjfTimelineIncludesInitialIdleTime() {
  SJFScheduler scheduler(false);
  scheduler.addProcess(Process(1, 3, 2));
  scheduler.runOffline();

  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(-1, 0, 3), ExecutionRecord(1, 3, 5)},
                 "SJF initial-idle timeline");
}

void testRoundRobinClampsInvalidQuantum() {
  RoundRobinScheduler scheduler(0);
  scheduler.addProcess(Process(1, 0, 2));
  scheduler.addProcess(Process(2, 0, 2));
  scheduler.runOffline();

  expectTimeline(scheduler.getGanttChart(),
                 {ExecutionRecord(1, 0, 1), ExecutionRecord(2, 1, 2),
                  ExecutionRecord(1, 2, 3), ExecutionRecord(2, 3, 4)},
                 "Round Robin invalid-quantum fallback timeline");
}

} // namespace

int main() {
  testFcfs();
  testSjfNonPreemptive();
  testSjfPreemptive();
  testPriorityNonPreemptive();
  testPriorityPreemptive();
  testRoundRobin();
  testSjfEqualBurstUsesArrivalOrder();
  testPriorityTieUsesArrivalOrder();
  testSjfTimelineIncludesInitialIdleTime();
  testRoundRobinClampsInvalidQuantum();

  if (failures != 0) {
    std::cerr << failures << " scheduler regression assertion(s) failed.\n";
    return 1;
  }

  std::cout << "All scheduler regression tests passed.\n";
  return 0;
}