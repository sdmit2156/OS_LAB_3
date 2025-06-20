#pragma once
#include <windows.h>
#include <vector>

struct MarkerParams {
    int id;
    int* array;
    int size;
    HANDLE startEvent;
    HANDLE resumeEvent;
    HANDLE terminateEvent;
    HANDLE notifyMain;
    CRITICAL_SECTION* arrayLock;
    CRITICAL_SECTION* consoleLock;
    std::vector<int>* markedIndices;
    bool* isFinished;
};

DWORD WINAPI MarkerThread(LPVOID lpParam);
