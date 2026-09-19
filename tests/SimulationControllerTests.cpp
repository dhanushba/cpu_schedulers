#include "SimulationController.h"

#include <cmath>
#include <iostream>
#include <string>

namespace {

int failures = 0;

void expect(bool condition, const std::string &description) {
  if (condition)
    return;
  std::cerr << "FAIL: " << description << '\n';
  failures++;
}

void expectNear(double actual, double expected,
                const std::string &description) {
  expect(std::abs(actual - expected) < 0.0001, description);
}

void testCompletedSimulationReportsIdle() {
  SimulationController controller;
  controller.addProcessRequest(5, 1, 0);
  controller.runOfflineInstant();

  expect(controller.currentTime() == 5, "simulation time is 5");
  expect(controller.currentRunningPid() == -1,
         "CPU is idle after completion");

  const auto timeline = controller.ganttChart();
  expect(timeline.size() == 1 && timeline.front().pid == 1 &&
             timeline.front().startTime == 0 && timeline.front().endTime == 5,
         "final Gantt interval is preserved");
}

      void testDisplayedWorkloadMetrics() {
        SimulationController controller;
        controller.addProcessRequest(3, 1, 0);
        controller.addProcessRequest(5, 1, 0);

        expect(controller.currentTime() == 0, "staged workload starts at time zero");
        expect(controller.currentRunningPid() == -1,
          "CPU is idle before execution starts");
        expectNear(controller.averageWaitingTime(), 0.0,
              "initial average waiting time is zero");
        expectNear(controller.averageTurnaroundTime(), 0.0,
              "initial average turnaround time is zero");

        controller.runOfflineInstant();

        expect(controller.currentTime() == 8, "displayed workload completes at time 8");
        expect(controller.currentRunningPid() == -1,
          "CPU is idle after displayed workload completes");
        expectNear(controller.averageWaitingTime(), 1.5,
              "displayed FCFS average waiting time");
        expectNear(controller.averageTurnaroundTime(), 5.5,
              "displayed FCFS average turnaround time");
      }

            void testEditsOnlyStagedProcesses() {
              SimulationController controller;
              controller.addProcessRequest(3, 1, 0);
              controller.editProcessRequest(1, 7, 4, 2);

              auto processes = controller.processes();
              expect(processes.size() == 1 && processes[0].pid == 1,
                "editing preserves the process PID");
              expect(processes[0].burstTime == 7 && processes[0].priority == 4 &&
                    processes[0].arrivalTime == 2,
                "editing updates staged process fields");

              controller.stepTick();
              controller.editProcessRequest(1, 9, 6, 0);
              processes = controller.processes();
              expect(processes[0].burstTime == 7 && processes[0].priority == 4 &&
                    processes[0].arrivalTime == 2,
                "editing is rejected after execution initializes");
            }

void testSnapshotsCaptureDecisionsAndReadyProcesses() {
  SimulationController controller;
  controller.setAlgorithm(1);
  controller.setPreemptiveMode(true);
  controller.addProcessRequest(4, 1, 0);
  controller.addProcessRequest(1, 1, 1);
  controller.runOfflineInstant();

  const auto snapshots = controller.snapshots();
  expect(snapshots.size() == 5, "one snapshot is recorded per tick");
  expect(snapshots[0].time == 0 && snapshots[0].runningPid == 1 &&
             snapshots[0].readyPids.empty(),
         "first SJF decision captures P1 running alone");
  expect(snapshots[1].time == 1 && snapshots[1].runningPid == 2 &&
             snapshots[1].readyPids == std::vector<int>{1},
         "preemptive decision captures P2 running with P1 ready");
  expect(snapshots[1].decision.find("shortest remaining time") !=
             std::string::npos,
         "snapshot explains the SJF selection rule");

  controller.resetRunKeepProcesses();
  expect(controller.snapshots().empty(), "reset clears snapshot history");
}

} // namespace

int main() {
  testCompletedSimulationReportsIdle();
  testDisplayedWorkloadMetrics();
  testEditsOnlyStagedProcesses();
  testSnapshotsCaptureDecisionsAndReadyProcesses();

  if (failures == 0)
    std::cout << "Simulation controller regression tests passed.\n";
  return failures == 0 ? 0 : 1;
}