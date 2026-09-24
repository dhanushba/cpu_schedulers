#include "FCFSScheduler.h"
#include <algorithm>
using namespace std;

FCFSScheduler::FCFSScheduler() : currentTime(0), isSorted(false) {}

void FCFSScheduler::addProcess(const Process& p) {
    processes.push_back(p);
    isSorted = false;
}

void FCFSScheduler::removeProcess(int pid) {
for (int i = 0; i < processes.size(); i++) {
    if (processes[i].pid == pid) {
        processes.erase(processes.begin() + i);
        break;
    }
}
}

bool FCFSScheduler::isFinished() const {
    for (const Process& p : processes) {
        if (!p.isFinished)
            return false;
    }
    return true;
}
//
void FCFSScheduler::tick() {

    if (!isSorted) { //sorting by arrival time
    sort(processes.begin(), processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrivalTime == b.arrivalTime
                ? a.pid < b.pid
                : a.arrivalTime < b.arrivalTime;
        });
    isSorted = true;
}

    Process* current = nullptr;

    for (Process& p : processes) {
        if (p.arrivalTime <= currentTime && !p.isFinished) {
            current = &p;
            break;
        }
    }

    if (current) {
        if (!current->hasStarted) {
            current->hasStarted = true;
        }

        current->remainingTime--;

    if (!ganttChart.empty() && ganttChart.back().pid == current->pid)
        {
    ganttChart.back().endTime++;
        }
    else
        {
        ganttChart.emplace_back(current->pid, currentTime, currentTime + 1);
        }
       //
        if (current->remainingTime == 0) {
            current->isFinished = true;
            current->completionTime = currentTime + 1;

            current->turnaroundTime =
                current->completionTime - current->arrivalTime;

            current->waitingTime =
                current->turnaroundTime - current->burstTime;
        }
    } else {
    if (!ganttChart.empty() && ganttChart.back().pid == -1) {
    ganttChart.back().endTime++;
}
 else {
    ganttChart.emplace_back(-1, currentTime, currentTime + 1);
}
    }

    currentTime++;
}
//
void FCFSScheduler::runOffline() {
    ganttChart.clear();
if (!isSorted) {
    sort(processes.begin(), processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrivalTime == b.arrivalTime
                ? a.pid < b.pid
                : a.arrivalTime < b.arrivalTime;
        });
    isSorted = true;
}
    currentTime = 0;

    for (Process& p : processes) {
        if (currentTime < p.arrivalTime)
            currentTime = p.arrivalTime;

        p.hasStarted = true;

        ganttChart.emplace_back(p.pid, currentTime, currentTime + p.burstTime);

        currentTime += p.burstTime;

        p.isFinished = true;
        p.completionTime = currentTime;

        p.turnaroundTime =
            p.completionTime - p.arrivalTime;

        p.waitingTime =
            p.turnaroundTime - p.burstTime;

        p.remainingTime = 0;
    }
}

vector<Process> FCFSScheduler::getProcesses() const {
    return processes;
}

vector<ExecutionRecord> FCFSScheduler::getGanttChart() const {
    return ganttChart;
}

double FCFSScheduler::getAverageWaitingTime() const {
    double total = 0;

    for (const Process& p : processes)
        total += p.waitingTime;

    return processes.empty() ? 0 : total / processes.size();
}

double FCFSScheduler::getAverageTurnaroundTime() const {
    double total = 0;

    for (const auto& p : processes)
        total += p.turnaroundTime;

    return processes.empty() ? 0 : total / processes.size();
}