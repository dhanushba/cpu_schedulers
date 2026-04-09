#pragma once
#include <vector>
#include "Process.h"
#include "ExecutionRecord.h"

class IScheduler {
public:
    virtual ~IScheduler() = default;

    // Core execution
    virtual void addProcess(const Process& p) = 0;
    virtual void tick() = 0; 
    virtual void runOffline() = 0; 
    
    // State checking
    virtual bool isFinished() const = 0;

    // Data retrieval
    virtual std::vector<Process> getProcesses() const = 0;
    virtual std::vector<ExecutionRecord> getGanttChart() const = 0;

    // Statistics
    virtual double getAverageWaitingTime() const = 0;
    virtual double getAverageTurnaroundTime() const = 0;
};