#include "Process.h"

Process::Process(int id, int arrival, int burst, int prio)
    : pid(id), arrivalTime(arrival), burstTime(burst), priority(prio),
      remainingTime(burst), completionTime(0), waitingTime(0), turnaroundTime(0),
      hasStarted(false), isFinished(false) {}
