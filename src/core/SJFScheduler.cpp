#include<iostream>
#include<algorithm>
#include "SJFScheduler.h"
using namespace std;

SJFScheduler::SJFScheduler(bool preemptive) : 
     currentTime(0), isPreemptive(preemptive), 
     currentRunningProcessIndex(-1), //idel CPU 
     currentProcessStartTime(0) {}

void SJFScheduler::addProcess(Process& p){
    p.remainingTime = p.burstTime;
    processes.push_back(p);
}


void SJFScheduler::tick(){
    //BY REFERENCE 
    for(auto &p : processes){ //currenttime is 0 at constructing the sched
         if(p.arrivalTime == currentTime)  // for every new arrived process
         {
            priorityQ.push(p);   //ready processes
         }
    }
    
    //if cpu is idel, run the top process in priority q
    if(!priorityQ.empty() && currentRunningProcessIndex == -1) {  
        currentRunningProcessIndex = getTopProcessIndex();
        priorityQ.pop();

        if(processes[currentRunningProcessIndex].hasStarted == 0){
            processes[currentRunningProcessIndex].hasStarted = 1;
            //currentProcessStartTime = currentTime;
        }
    }

    //if cpu is not idle, updating time is needed
    if(currentRunningProcessIndex != -1){
        processes[currentRunningProcessIndex].remainingTime--; // but still in PQ 
        if(processes[currentRunningProcessIndex].remainingTime == 0){ 
            //finished //cpu is idle now
            processes[currentRunningProcessIndex].isFinished = 1;
            processes[currentRunningProcessIndex].completionTime = currentTime + 1;//it's the last tick
            processes[currentRunningProcessIndex].turnaroundTime = processes[currentRunningProcessIndex].completionTime - processes[currentRunningProcessIndex].arrivalTime; 
            processes[currentRunningProcessIndex].waitingTime = processes[currentRunningProcessIndex].turnaroundTime - processes[currentRunningProcessIndex].burstTime;
            currentRunningProcessIndex = -1; 
        }
    }
 
    //not finished yet:
    if(currentRunningProcessIndex != -1 && isPreemptive && !priorityQ.empty()){ //comparision needed
        if(processes[getTopProcessIndex()].remainingTime < processes[currentRunningProcessIndex].remainingTime){
            priorityQ.push(processes[currentRunningProcessIndex]); //pushed to wait again
            currentRunningProcessIndex = getTopProcessIndex();
            priorityQ.pop();
            
             if(processes[currentRunningProcessIndex].hasStarted == 0){
                processes[currentRunningProcessIndex].hasStarted = 1;
                //currentProcessStartTime = currentTime;
            }
        }
    }
    
    if(currentRunningProcessIndex != -1){
    //to not slice the same process:
        if(!ganttChart.empty() && ganttChart.back().pid == processes[currentRunningProcessIndex].pid)
            ganttChart.back().endTime = currentTime + 1;
        else
            ganttChart.push_back(ExecutionRecord (processes[currentRunningProcessIndex].pid, currentTime, currentTime + 1));
    }
    
    currentTime++;
}


int SJFScheduler::getTopProcessIndex(){
    for (int i = 0; i < processes.size(); i++)
    {
        if(processes[i].pid == priorityQ.top().pid){
            return i;
        }
    }
}

bool SJFScheduler::isFinished() const{
    if(processes.empty()) return true;
    for(auto &p : processes){
        if(!p.isFinished)
            return false;
    }
    return true;
}

void SJFScheduler::runOffline(){
    while(!isFinished())
        tick();
}

vector<Process> SJFScheduler::getProcesses() const{
    return processes;
}
vector<ExecutionRecord> SJFScheduler::getGanttChart() const{
    return ganttChart; 
}


double SJFScheduler::getAverageWaitingTime() const{
    if(processes.empty()) return 0;
    double totalWaitingTime = 0;
    for(auto &p : processes){
        totalWaitingTime += p.waitingTime;
    }

    return totalWaitingTime / processes.size();
}
double SJFScheduler::getAverageTurnaroundTime() const {
    if(processes.empty()) return 0;
    double totalTurnaroundTime = 0;
    for(auto &p : processes){
        totalTurnaroundTime += p.turnaroundTime;
    }

    return totalTurnaroundTime / processes.size();
}


void SJFScheduler::removeProcess(int pid){
    for (int i = 0; i < processes.size(); i++)
    {
        if(processes[i].pid == pid){
            if(currentRunningProcessIndex == i) currentRunningProcessIndex = -1;
            else if(currentRunningProcessIndex > i) currentRunningProcessIndex--; //shifting
            processes.erase(processes.begin() + i);
            //so we need to erase it from PQ too
            std::priority_queue<Process, std::vector<Process>, std::greater<Process>> priorityQ2;
            while (!priorityQ.empty()){
                if(priorityQ.top().pid != pid) {
                    priorityQ2.push(priorityQ.top());
                    }
                    priorityQ.pop();
                }
            priorityQ = priorityQ2; //update it
            break;
        }
    } 
}
