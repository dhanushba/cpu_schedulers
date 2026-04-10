#include<bits/stdc++.h>
#include "C:\computer programming\OS_project\cpu_schedulers\include\core\PriorityScheduler.h"
using namespace std;
//constructor 
explicit PriorityScheduler::PriorityScheduler(bool preemptive):currentTime(0),isPreemptive(preemptive),currentProcessStartTime(0),currentRunningProcessIndex(-1){}
//add process in the vector
void PriorityScheduler::addProcess( const Process &p){
    Process new_process=p;
   new_process.remainingTime=new_process.burstTime;
   processes.push_back(new_process);
}
           //TICK
void  PriorityScheduler::tick(){
    //add the processes in the PRIORITY QUEUE THAT MATCHES THE SAME CURRENT TIME NOW
         for(int i=0;i<processes.size();++i){
        if(processes[i].arrivalTime==currentTime){
            priorityQueue.push(processes[i]);//add it now in the ready queue
        } }
    //if CPU IS IDLE
    //That means current index=-1 
    if(currentRunningProcessIndex==-1){
     //cpu is idle now
    //if the queue not empty 
     if(!priorityQueue.empty()){
     Process Topprocess=priorityQueue.top();//it holds the process working RIGHT NOW
     priorityQueue.pop();//remove it 
    for(int i=0;i<processes.size();++i){
            if(Topprocess.pid==processes[i].pid){
            currentRunningProcessIndex=i;//it actually holds the process that's working on cpu to hold it's index in the vector
            //so that if i want to access the current process through the current process index
            break;}}
           currentProcessStartTime=currentTime;//the start time of process
            //REMAIN TIME OF PROCESS=BURST AT FIRST AND THEN IT WILL BE DECREMENETED
           // processes[currentRunningProcessIndex].remainingTime=processes[currentRunningProcessIndex].burstTime;}
           }}
             ///////////if NOT IDLE
else{ 
     processes[currentRunningProcessIndex].remainingTime--;//DECREMENT PROCESS WORKING RIGHT NOW

     //if the remaining time ==0 in any case preemptive or nott
     if(processes[currentRunningProcessIndex].remainingTime==0){
        //this process has just finished id+start+end
        ganttChart.push_back(ExecutionRecord(processes[currentRunningProcessIndex].pid,currentProcessStartTime,currentTime+1));
        //return cpu to IDLE AGAIN
        currentRunningProcessIndex=-1;//IDLE CPU AGAIN!
     }
     //check it's preemptive and queue not empty
     else if(isPreemptive&&!priorityQueue.empty()){
      if(priorityQueue.top().priority<processes[currentRunningProcessIndex].priority){
        //add it on the ganttchart
        ganttChart.push_back(ExecutionRecord(processes[currentRunningProcessIndex].pid,currentProcessStartTime,currentTime+1));
        //the process exit in the queue higher in priority than the current running process
        //add the current process in the queue
        priorityQueue.push(processes[currentRunningProcessIndex]);//add the process in the queue
        Process new_process=priorityQueue.top();//the process that need to be executed 
        //KNOW THE NEW PROCESS LOCATION IN THE VECTOR ORIGINAL
        for(int i=0;i<processes.size();++i){
            if(new_process.pid==processes[i].pid){
                currentRunningProcessIndex=i;
                break;
            }}
            currentProcessStartTime=currentTime+1;
            priorityQueue.pop();//remove it it works now on cpu we don't need it in QUEUE
      }}
     
             /*ADAM'S LOGIC FOR NON PREEMPTIVE*/
    }


currentTime++;
}



