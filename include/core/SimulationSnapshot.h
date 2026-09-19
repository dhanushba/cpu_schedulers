#pragma once

#include <string>
#include <vector>

struct SimulationSnapshot {
  int time;
  int runningPid;
  std::vector<int> readyPids;
  std::string decision;
};