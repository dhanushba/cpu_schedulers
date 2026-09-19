#include "SimulationController.h"

#include "FCFSScheduler.h"
#include "PriorityScheduler.h"
#include "RoundRobinScheduler.h"
#include "SJFScheduler.h"

#include <algorithm>
#include <string>

SimulationController::SimulationController(QObject *parent)
    : QObject(parent), selectedAlgorithm(Algorithm::FCFS), preemptiveMode(true),
      timeQuantum(2), now(0), nextPid(1), runningPid(-1), avgWaiting(0.0),
      avgTurnaround(0.0), paused(false) {
  timer.setInterval(1000);
  connect(&timer, &QTimer::timeout, this, &SimulationController::onTick);
}

std::vector<Process> SimulationController::processes() const {
  if (scheduler)
    return scheduler->getProcesses();
  return stagedProcesses;
}

std::vector<ExecutionRecord> SimulationController::ganttChart() const {
  return displayTimeline;
}

std::vector<SimulationSnapshot> SimulationController::snapshots() const {
  return simulationSnapshots;
}

int SimulationController::currentTime() const { return now; }

int SimulationController::currentRunningPid() const { return runningPid; }

double SimulationController::averageWaitingTime() const { return avgWaiting; }

double SimulationController::averageTurnaroundTime() const {
  return avgTurnaround;
}

bool SimulationController::hasScheduler() const { return scheduler != nullptr; }

bool SimulationController::isLiveRunning() const { return timer.isActive(); }

bool SimulationController::isPaused() const { return paused; }

void SimulationController::setAlgorithm(int algorithmIndex) {
  if (scheduler)
    return;
  if (algorithmIndex < 0 || algorithmIndex > 3)
    return;
  selectedAlgorithm = static_cast<Algorithm>(algorithmIndex);
  emit stateUpdated();
}

void SimulationController::setPreemptiveMode(bool enabled) {
  if (scheduler)
    return;
  preemptiveMode = enabled;
  emit stateUpdated();
}

void SimulationController::setQuantum(int q) {
  if (scheduler)
    return;
  timeQuantum = q > 0 ? q : 1;
  emit stateUpdated();
}

void SimulationController::startLive() {
  if (timer.isActive())
    return;

  ensureSchedulerInitialized();

  paused = false;
  timer.start();
  emit modeUpdated(true, false, hasScheduler());
  emit stateUpdated();
}

void SimulationController::stepTick() {
  if (timer.isActive())
    return;
  ensureSchedulerInitialized();
  if (!scheduler)
    return;
  const auto processesBeforeTick = scheduler->getProcesses();
  scheduler->tick();
  now++;
  rebuildDisplayTimeline();
  recordSnapshot(processesBeforeTick);
  refreshDerivedState();
  emit modeUpdated(false, false, true);
  emit stateUpdated();
}

void SimulationController::runOfflineInstant() {
  if (timer.isActive())
    return;
  ensureSchedulerInitialized();
  if (!scheduler)
    return;

  while (!scheduler->isFinished()) {
    const auto processesBeforeTick = scheduler->getProcesses();
    scheduler->tick();
    now++;
    rebuildDisplayTimeline();
    recordSnapshot(processesBeforeTick);
  }

  refreshDerivedState();
  emit modeUpdated(false, false, true);
  emit stateUpdated();
}

void SimulationController::stopSimulation() {
  timer.stop();
  paused = false;
  refreshDerivedState();
  emit modeUpdated(false, false, hasScheduler());
  emit stateUpdated();
}

void SimulationController::clearAllProcesses() {
  timer.stop();
  paused = false;
  scheduler.reset();
  stagedProcesses.clear();
  displayTimeline.clear();
  simulationSnapshots.clear();
  now = 0;
  nextPid = 1;
  runningPid = -1;
  avgWaiting = 0.0;
  avgTurnaround = 0.0;
  emit modeUpdated(false, false, false);
  emit stateUpdated();
}

void SimulationController::resetRunKeepProcesses() {
  timer.stop();
  paused = false;

  const auto current = processes();
  std::vector<Process> resetProcesses;
  resetProcesses.reserve(current.size());

  int maxPid = 0;
  for (const auto &p : current) {
    resetProcesses.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority);
    if (p.pid > maxPid)
      maxPid = p.pid;
  }

  scheduler.reset();
  stagedProcesses = std::move(resetProcesses);
  displayTimeline.clear();
  simulationSnapshots.clear();
  now = 0;
  runningPid = -1;
  avgWaiting = 0.0;
  avgTurnaround = 0.0;
  nextPid = maxPid > 0 ? (maxPid + 1) : 1;

  emit modeUpdated(false, false, false);
  emit stateUpdated();
}

void SimulationController::pauseResumeLive() {
  if (!scheduler)
    return;

  if (!timer.isActive()) {
    timer.start();
    paused = false;
    emit modeUpdated(true, false, true);
    emit stateUpdated();
    return;
  }

  timer.stop();
  paused = true;
  emit modeUpdated(false, true, true);
  emit stateUpdated();
}

void SimulationController::addProcessRequest(int burst, int priority, int arrival) {
  if (burst <= 0 || arrival < 0)
    return;

  Process p(nextPid++, arrival, burst, priority);

  if (!scheduler) {
    stagedProcesses.push_back(p);
    refreshDerivedState();
    emit stateUpdated();
    return;
  }

  scheduler->addProcess(p);
  refreshDerivedState();
  rebuildDisplayTimeline();
  emit stateUpdated();
}

void SimulationController::deleteProcessRequest(int pid) {
  if (!scheduler) {
    for (auto it = stagedProcesses.begin(); it != stagedProcesses.end(); ++it) {
      if (it->pid == pid) {
        stagedProcesses.erase(it);
        break;
      }
    }
    refreshDerivedState();
    emit stateUpdated();
    return;
  }

  scheduler->removeProcess(pid);
  refreshDerivedState();
  rebuildDisplayTimeline();
  emit stateUpdated();
}

void SimulationController::onTick() {
  if (!scheduler)
    return;

  // Intentionally keep ticking forever to allow live additions while idle.
  const auto processesBeforeTick = scheduler->getProcesses();
  scheduler->tick();
  now++;
  rebuildDisplayTimeline();
  recordSnapshot(processesBeforeTick);
  refreshDerivedState();
  emit modeUpdated(true, false, true);
  emit stateUpdated();
}

void SimulationController::ensureSchedulerInitialized() {
  if (scheduler)
    return;

  createScheduler();
  if (!scheduler)
    return;

  for (const auto &p : stagedProcesses) {
    scheduler->addProcess(p);
  }

  stagedProcesses.clear();
  refreshDerivedState();
  rebuildDisplayTimeline();
}

void SimulationController::refreshDerivedState() {
  runningPid = -1;
  avgWaiting = 0.0;
  avgTurnaround = 0.0;

  if (!scheduler)
    return;

  if (!scheduler->isFinished() && !displayTimeline.empty()) {
    const auto &last = displayTimeline.back();
    if (last.endTime == now) {
      runningPid = last.pid;
    }
  }

  avgWaiting = scheduler->getAverageWaitingTime();
  avgTurnaround = scheduler->getAverageTurnaroundTime();
}

void SimulationController::rebuildDisplayTimeline() {
  displayTimeline.clear();
  if (!scheduler)
    return;

  const auto records = scheduler->getGanttChart();
  displayTimeline = records;

  if (now <= 0)
    return;

  if (displayTimeline.empty() || displayTimeline.back().endTime < now) {
    if (!displayTimeline.empty() && displayTimeline.back().pid == -1) {
      displayTimeline.back().endTime = now;
    } else {
      const int start = displayTimeline.empty() ? 0 : displayTimeline.back().endTime;
      displayTimeline.emplace_back(-1, start, now);
    }
  }
}

void SimulationController::recordSnapshot(
    const std::vector<Process> &processesBeforeTick) {
  int executedPid = -1;
  if (!displayTimeline.empty()) {
    const auto &last = displayTimeline.back();
    if (last.startTime < now && last.endTime >= now)
      executedPid = last.pid;
  }

  std::vector<int> readyPids;
  const int decisionTime = now - 1;
  for (const auto &process : processesBeforeTick) {
    if (!process.isFinished && process.remainingTime > 0 &&
        process.arrivalTime <= decisionTime && process.pid != executedPid) {
      readyPids.push_back(process.pid);
    }
  }
  std::sort(readyPids.begin(), readyPids.end());

  simulationSnapshots.push_back(
      {decisionTime, executedPid, readyPids, decisionForPid(executedPid)});
}

std::string SimulationController::decisionForPid(int pid) const {
  if (pid < 0)
    return "CPU idle: no process was ready";

  const std::string processName = "P" + std::to_string(pid) + " ran: ";
  switch (selectedAlgorithm) {
  case Algorithm::FCFS:
    return processName + "earliest arrival time";
  case Algorithm::SJF:
    return processName + (preemptiveMode ? "shortest remaining time"
                                         : "shortest ready job");
  case Algorithm::Priority:
    return processName + "highest priority among ready processes";
  case Algorithm::RoundRobin:
    return processName + "next in Round Robin rotation (quantum " +
           std::to_string(timeQuantum) + ")";
  }

  return processName + "selected by scheduler";
}

void SimulationController::createScheduler() {
  switch (selectedAlgorithm) {
  case Algorithm::FCFS:
    scheduler = std::make_unique<FCFSScheduler>();
    break;
  case Algorithm::SJF:
    scheduler = std::make_unique<SJFScheduler>(preemptiveMode);
    break;
  case Algorithm::Priority:
    scheduler = std::make_unique<PriorityScheduler>(preemptiveMode);
    break;
  case Algorithm::RoundRobin:
    scheduler = std::make_unique<RoundRobinScheduler>(timeQuantum > 0 ? timeQuantum : 1);
    break;
  }
}
