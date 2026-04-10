#pragma once
#include "IScheduler.h"
#include <vector>
#include <queue>
#include <functional>

class PriorityScheduler : public IScheduler {
private:
    std::vector<Process> processes;
    std::vector<ExecutionRecord> ganttChart;
    int currentTime;
    bool isPreemptive;
    //definition to let process work with minimum heap needed in the implementation
    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> priorityQueue;
    
    // Tracking variables
    int currentRunningProcessIndex;
    int currentProcessStartTime;
   

public:
    explicit PriorityScheduler(bool preemptive);
    ~PriorityScheduler() override = default;

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
//operator need to let greater work based on what I need 
 bool operator>(const Process& other,const Process &p)  {
        return p.priority > other.priority;
    }

