#pragma once

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    
    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    
    bool hasStarted;
    bool isFinished;

    // Constructor declaration
    Process(int id, int arrival, int burst, int prio = 0);
};