#pragma once

#include <QObject>
#include <QTimer>
#include <memory>
#include <vector>

#include "ExecutionRecord.h"
#include "IScheduler.h"
#include "Process.h"
#include "SimulationSnapshot.h"

class SimulationController : public QObject {
  Q_OBJECT

public:
  enum class Algorithm {
    FCFS = 0,
    SJF = 1,
    Priority = 2,
    RoundRobin = 3,
  };

  explicit SimulationController(QObject *parent = nullptr);

  std::vector<Process> processes() const;
  std::vector<ExecutionRecord> ganttChart() const;
  std::vector<SimulationSnapshot> snapshots() const;
  int currentTime() const;
  int currentRunningPid() const;
  double averageWaitingTime() const;
  double averageTurnaroundTime() const;

  bool hasScheduler() const;
  bool isLiveRunning() const;
  bool isPaused() const;

public slots:
  void setAlgorithm(int algorithmIndex);
  void setPreemptiveMode(bool enabled);
  void setQuantum(int q);

  void startLive();
  void stepTick();
  void runOfflineInstant();
  void stopSimulation();
  void pauseResumeLive();
  void resetRunKeepProcesses();
  void clearAllProcesses();
  void addProcessRequest(int burst, int priority, int arrival);
  void editProcessRequest(int pid, int burst, int priority, int arrival);
  void deleteProcessRequest(int pid);

signals:
  void stateUpdated();
  void modeUpdated(bool liveRunning, bool paused, bool hasScheduler);

private slots:
  void onTick();

private:
  void ensureSchedulerInitialized();
  void refreshDerivedState();
  void rebuildDisplayTimeline();
  void recordSnapshot(const std::vector<Process> &processesBeforeTick);
  std::string decisionForPid(int pid) const;
  void createScheduler();

  QTimer timer;
  std::unique_ptr<IScheduler> scheduler;
  std::vector<Process> stagedProcesses;
  std::vector<ExecutionRecord> displayTimeline;
  std::vector<SimulationSnapshot> simulationSnapshots;

  Algorithm selectedAlgorithm;
  bool preemptiveMode;
  int timeQuantum;

  int now;
  int nextPid;
  int runningPid;
  double avgWaiting;
  double avgTurnaround;
  bool paused;
};
