#include "PriorityScheduler.h"
#include <bits/stdc++.h>
using namespace std;
// constructor
//COMPLETE IMPLEMENTATION
PriorityScheduler::PriorityScheduler(bool preemptive)
    : currentTime(0), isPreemptive(preemptive), currentProcessStartTime(0),
      currentRunningProcessIndex(-1) {}
// add process in the vector
void PriorityScheduler::addProcess(const Process &p) {
  Process new_process = p;
  new_process.remainingTime = new_process.burstTime;
  processes.push_back(new_process);
}
// TICK
void PriorityScheduler::tick() {
    if(isFinished()) return;//scheduler has ended

  // add the processes in the PRIORITY QUEUE THAT MATCHES THE SAME CURRENT TIME
  // NOW
  for (int i = 0; i < processes.size(); ++i) {
    if (!processes[i].hasStarted && !processes[i].isFinished &&processes[i].arrivalTime == currentTime) {
      priorityQueue.push(processes[i]); // add it now in the ready queue
    }
  }
  

  
  // if CPU IS IDLE
  // That means current index=-1
  if (currentRunningProcessIndex == -1) {
    // cpu is idle now
    // if the queue not empty
    if (!priorityQueue.empty()) {
      Process Topprocess =
          priorityQueue.top(); // it holds the process working RIGHT NOW
      priorityQueue.pop();     // remove it
      for (int i = 0; i < processes.size(); ++i) {
        if (Topprocess.pid == processes[i].pid) {
          currentRunningProcessIndex =i; // it actually holds the process that's working on cpu to hold
          // it's index in the vector
          // so that if i want to access the current process through the current
          // process index
          break;
        }
      }
    }
  }
  ///////////if NOT IDLE
  else {
    // check it's preemptive and queue not empty
    if (isPreemptive && !priorityQueue.empty()) {
      if (priorityQueue.top().priority < processes[currentRunningProcessIndex].priority) {
        // the process exit in the queue higher in priority than the current
        // running process add the current process in the queue
        priorityQueue.push(processes[currentRunningProcessIndex]); // add the process in the  queue
        Process new_process =priorityQueue.top(); // the process that need to be executed
        priorityQueue.pop(); // remove it it works now on cpu we don't need it in QUEUE
        // KNOW THE NEW PROCESS LOCATION IN THE VECTOR ORIGINAL
        for (int i = 0; i < processes.size(); ++i) {
          if (new_process.pid == processes[i].pid) {
            currentRunningProcessIndex = i;
            break;
          }
        }
      }
    }
    /*ADAM'S LOGIC FOR NON PREEMPTIVE*/
  }

  int currentPID = -1;
  if (currentRunningProcessIndex != -1) {
      currentPID = processes[currentRunningProcessIndex].pid;
      processes[currentRunningProcessIndex].remainingTime--;
      processes[currentRunningProcessIndex].hasStarted = true;
  }


  // GANTTCHART IMPLEMENTATION
  if (!ganttChart.empty() && ganttChart.back().pid == currentPID) {
    ganttChart.back().endTime =currentTime + 1; // stretching the interval of this process extra time
  } else {
    // no this is NEW PROCESS ADD IT
    ganttChart.push_back( ExecutionRecord(currentPID, currentTime, currentTime + 1));
  }


  // if the remaining time ==0 in any case preemptive or nott
  if (currentRunningProcessIndex != -1 && processes[currentRunningProcessIndex].remainingTime == 0) {
     //first we need to computer everything about this process
     processes[currentRunningProcessIndex].completionTime=currentTime+1;//it will finish it's round in the next tick 
     processes[currentRunningProcessIndex].turnaroundTime=processes[currentRunningProcessIndex].completionTime-processes[currentRunningProcessIndex].arrivalTime;
     processes[currentRunningProcessIndex].waitingTime=processes[currentRunningProcessIndex].turnaroundTime-processes[currentRunningProcessIndex].burstTime;

      // return cpu to IDLE AGAIN
      processes[currentRunningProcessIndex].isFinished=true;
      currentRunningProcessIndex = -1; // IDLE CPU AGAIN!
  }

  currentTime++;
}
//cout processes 
 vector<Process> PriorityScheduler:: getProcesses()const{
  return processes;
}
//cout ganttchart
vector<ExecutionRecord> PriorityScheduler:: getGanttChart() const{
  return ganttChart;
}
//run offline
 void PriorityScheduler::runOffline(){
  while(!isFinished()){
    tick();//call this everytime 
  }
 }
 //is finished method
 bool PriorityScheduler::isFinished()const{
  if(processes.empty())return true;//all processes has ended
  for(auto &p:processes){
    if(!p.isFinished){
      return false;//there is still process hasn't finished so scheduler is active
    }
  }
  //NO WE HAVE FINISHED ALL PROCESSES
  return true;}
  //AVERAGE WAITING TIME
  double PriorityScheduler:: getAverageWaitingTime() const{
    if(processes.empty()) return 0;
 double waitingtime=0;
 for(auto &p: processes){
  waitingtime+= p.waitingTime;
 }
 return waitingtime/processes.size();} 
 //AVERAGE TURN AROUND
 double  PriorityScheduler::getAverageTurnaroundTime() const{
   if(processes.empty()) return 0;
 double turnaround=0;
 for(auto &p: processes){
  turnaround+= p.turnaroundTime;
 }
 return turnaround/processes.size();}
 /////////////////////////////////////////
 //remove process
  void PriorityScheduler:: removeProcess(int pid){
    if(processes.empty()) return;//nothing to be removed
    ////////////////////////
    else{
      for(int i=0;i<processes.size();++i){
        //first we need to search of this pid in the vector to know it's index
        if(processes[i].pid==pid){
          if(currentRunningProcessIndex==i){currentRunningProcessIndex=-1;}//return it to IDLE AGAIN
          else if(currentRunningProcessIndex>i){
            // because if we have running process at 7 and we remove one from 3 ,7 need to be 6 
            //what if the current running process is at 3 and the removed from 5 no decrement is needed
            currentRunningProcessIndex--;//decrement the index since we will remove one process from original vector
          }
          processes.erase(processes.begin()+i);//remove it from the vector 
          //what about the process we have removed from vector was also in the ready queue?
          //erase it from ready queue too
          //create another queue
           std::priority_queue<Process, std::vector<Process>, std::greater<Process>> priorityQueue_extra;
          while(!priorityQueue.empty()){
            if(priorityQueue.top().pid!=pid){
              //add all in this new priority queue except this pid
              priorityQueue_extra.push(priorityQueue.top());
              priorityQueue.pop();
            }}
            priorityQueue=priorityQueue_extra;//THIS NEW ONE WITHOUT THE EXACT PID "REMOVED ONE"

          break;}}
      }}
bool operator>(const Process& a, const Process& b) {
    return a.priority > b.priority;
}