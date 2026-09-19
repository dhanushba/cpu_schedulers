#include "SimulationController.h"

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
  testSnapshotsCaptureDecisionsAndReadyProcesses();

  if (failures == 0)
    std::cout << "Simulation controller regression tests passed.\n";
  return failures == 0 ? 0 : 1;
}