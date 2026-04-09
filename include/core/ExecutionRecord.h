#pragma once

struct ExecutionRecord {
    int pid;
    int startTime;
    int endTime;
    
    // Constructor declaration
    ExecutionRecord(int id, int start, int end);
};