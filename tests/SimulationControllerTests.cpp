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

        void testDeletesMultipleProcessesTogether() {
          SimulationController controller;
          controller.addProcessRequest(3, 1, 0);
          controller.addProcessRequest(4, 2, 1);
          controller.addProcessRequest(5, 3, 2);

          int updates = 0;
          QObject::connect(&controller, &SimulationController::stateUpdated,
                           [&updates]() { updates++; });
          controller.deleteProcessesRequest({1, 3});

          const auto processes = controller.processes();
          expect(processes.size() == 1 && processes[0].pid == 2,
                 "bulk delete removes every requested process");
          expect(updates == 1, "bulk delete refreshes the UI once");
        }

        void testPriorityConfigurationReachesScheduler() {
          SimulationController nonPreemptive;
          nonPreemptive.setAlgorithm(2);
          nonPreemptive.setPreemptiveMode(false);
          nonPreemptive.addProcessRequest(5, 2, 0);
          nonPreemptive.addProcessRequest(2, 1, 1);
          nonPreemptive.runOfflineInstant();

          const auto nonPreemptiveTimeline = nonPreemptive.ganttChart();
          expect(nonPreemptiveTimeline.size() == 2 &&
                     nonPreemptiveTimeline[0].pid == 1 &&
                     nonPreemptiveTimeline[0].startTime == 0 &&
                     nonPreemptiveTimeline[0].endTime == 5 &&
                     nonPreemptiveTimeline[1].pid == 2,
                 "non-preemptive priority runs the current process to completion");

          SimulationController preemptive;
          preemptive.setAlgorithm(2);
          preemptive.setPreemptiveMode(true);
          preemptive.addProcessRequest(5, 2, 0);
          preemptive.addProcessRequest(2, 1, 1);
          preemptive.runOfflineInstant();

          const auto preemptiveTimeline = preemptive.ganttChart();
          expect(preemptiveTimeline.size() == 3 &&
                     preemptiveTimeline[0].pid == 1 &&
                     preemptiveTimeline[0].startTime == 0 &&
                     preemptiveTimeline[0].endTime == 1 &&
                     preemptiveTimeline[1].pid == 2 &&
                     preemptiveTimeline[1].startTime == 1 &&
                     preemptiveTimeline[1].endTime == 3 &&
                     preemptiveTimeline[2].pid == 1,
                 "preemptive priority interrupts for the lower priority value");
        }

        void testAllAlgorithmSelectionsReachScheduler() {
          SimulationController fcfs;
          fcfs.addProcessRequest(4, 0, 0);
          fcfs.addProcessRequest(1, 0, 0);
          fcfs.runOfflineInstant();
          const auto fcfsTimeline = fcfs.ganttChart();
          expect(fcfsTimeline.size() == 2 && fcfsTimeline[0].pid == 1 &&
               fcfsTimeline[0].endTime == 4 && fcfsTimeline[1].pid == 2,
             "default algorithm index selects FCFS");

          SimulationController sjf;
          sjf.setAlgorithm(1);
          sjf.setPreemptiveMode(false);
          sjf.addProcessRequest(4, 0, 0);
          sjf.addProcessRequest(1, 0, 0);
          sjf.runOfflineInstant();
          const auto sjfTimeline = sjf.ganttChart();
          expect(sjfTimeline.size() == 2 && sjfTimeline[0].pid == 2 &&
               sjfTimeline[0].endTime == 1 && sjfTimeline[1].pid == 1,
             "algorithm index 1 selects non-preemptive SJF");

          SimulationController roundRobin;
          roundRobin.setAlgorithm(3);
          roundRobin.setQuantum(1);
          roundRobin.addProcessRequest(2, 0, 0);
          roundRobin.addProcessRequest(2, 0, 0);
          roundRobin.runOfflineInstant();
          const auto roundRobinTimeline = roundRobin.ganttChart();
          expect(roundRobinTimeline.size() == 4 &&
               roundRobinTimeline[0].pid == 1 &&
               roundRobinTimeline[1].pid == 2 &&
               roundRobinTimeline[2].pid == 1 &&
               roundRobinTimeline[3].pid == 2,
             "algorithm index 3 selects Round Robin with configured quantum");
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
  testDeletesMultipleProcessesTogether();
  testPriorityConfigurationReachesScheduler();
  testAllAlgorithmSelectionsReachScheduler();
  testSnapshotsCaptureDecisionsAndReadyProcesses();

  if (failures == 0)
    std::cout << "Simulation controller regression tests passed.\n";
  return failures == 0 ? 0 : 1;
}