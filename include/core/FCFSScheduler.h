#pragma once
#include "IScheduler.h"
#include <vector>

class FCFSScheduler : public IScheduler {
private:
    std::vector<Process> processes;
    std::vector<ExecutionRecord> ganttChart;
    int currentTime;
    
    // Add any private helper variables/queues you need here

public:
    FCFSScheduler();
    ~FCFSScheduler() override = default;

    void addProcess(const Process& p) override;
    void tick() override;
    void runOffline() override;
    bool isFinished() const override;

    std::vector<Process> getProcesses() const override;
    std::vector<ExecutionRecord> getGanttChart() const override;
    double getAverageWaitingTime() const override;
    double getAverageTurnaroundTime() const override;
};