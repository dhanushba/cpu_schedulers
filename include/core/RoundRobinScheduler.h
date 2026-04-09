#pragma once
#include "IScheduler.h"
#include <vector>

class RoundRobinScheduler : public IScheduler {
private:
    std::vector<Process> processes;
    std::vector<ExecutionRecord> ganttChart;
    int currentTime;
    int timeQuantum;
    
    // Tracking variables
    std::vector<int> readyQueue;
    int currentQuantumSpent;
    int currentRunningProcessIndex;
    int currentProcessStartTime;

public:
    explicit RoundRobinScheduler(int quantum);
    ~RoundRobinScheduler() override = default;

    void addProcess(const Process& p) override;
    void removeProcess(int pid) override;
    void tick() override;
    void runOffline() override;
    bool isFinished() const override;

    std::vector<Process> getProcesses() const override;
    std::vector<ExecutionRecord> getGanttChart() const override;
    double getAverageWaitingTime() const override;
    double getAverageTurnaroundTime() const override;
};
