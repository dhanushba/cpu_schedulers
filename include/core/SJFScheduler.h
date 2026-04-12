#pragma once
#include "IScheduler.h"
#include <vector>
#include <queue>

class SJFScheduler : public IScheduler {
private:
    std::vector<Process> processes; 
    std::vector<ExecutionRecord> ganttChart;

    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> priorityQ; 

    int currentTime;
    bool isPreemptive;
    
    // Tracking variables
    int currentRunningProcessIndex;
    int currentProcessStartTime;

public:
    explicit SJFScheduler(bool preemptive);
    ~SJFScheduler() override = default;

    void addProcess(const Process& p) override;
    void removeProcess(int pid) override;
    void tick() override;
    void runOffline() override;
    bool isFinished() const override;

    std::vector<Process> getProcesses() const override;
    std::vector<ExecutionRecord> getGanttChart() const override;
    double getAverageWaitingTime() const override;
    double getAverageTurnaroundTime() const override;

    //overriding greater operator to edit PQ sorting algo.
    bool operator>(const Process & RHS) const;
    int getTopProcessIndex();
};


    bool operator>(const Process & LHS, const Process & RHS) {
        return LHS.remainingTime < RHS.remainingTime; //sorting by least-remaining-time first
    }
  
    ///update changes////////
    //changes//