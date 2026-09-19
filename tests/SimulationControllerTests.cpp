#include "SimulationController.h"

#include <iostream>

int main() {
  SimulationController controller;
  controller.addProcessRequest(5, 1, 0);
  controller.runOfflineInstant();

  if (controller.currentTime() != 5) {
    std::cerr << "FAIL: expected simulation time 5, got "
              << controller.currentTime() << '\n';
    return 1;
  }

  if (controller.currentRunningPid() != -1) {
    std::cerr << "FAIL: expected CPU idle after completion, got P"
              << controller.currentRunningPid() << '\n';
    return 1;
  }

  const auto timeline = controller.ganttChart();
  if (timeline.size() != 1 || timeline.front().pid != 1 ||
      timeline.front().startTime != 0 || timeline.front().endTime != 5) {
    std::cerr << "FAIL: final Gantt interval changed unexpectedly\n";
    return 1;
  }

  std::cout << "Simulation controller regression tests passed.\n";
  return 0;
}