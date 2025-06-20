#include "marker.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

DWORD WINAPI MarkerThread(LPVOID lpParam) {
    MarkerParams* params = (MarkerParams*)lpParam;
    int id = params->id;
    int* arr = params->array;
    int size = params->size;
    srand(id);

    WaitForSingleObject(params->startEvent, INFINITE);
    while (true) {
        int marked = 0;
        while (true) {
            int rnd = rand();
            int index = rnd % size;

            EnterCriticalSection(params->arrayLock);
            if (arr[index] == 0) {
                Sleep(5);
                arr[index] = id;
                params->markedIndices->push_back(index);
                marked++;
                LeaveCriticalSection(params->arrayLock);
                Sleep(5);
            }
            else {
                LeaveCriticalSection(params->arrayLock);

                EnterCriticalSection(params->consoleLock);
                std::cout << "[Marker " << id << "] can't mark index " << index
                    << ", marked: " << marked << std::endl;
                LeaveCriticalSection(params->consoleLock);

                SetEvent(params->notifyMain);
                HANDLE waitHandles[2] = { params->resumeEvent, params->terminateEvent };
                DWORD result = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
                if (result == WAIT_OBJECT_0 + 1) {
                    EnterCriticalSection(params->arrayLock);
                    for (int idx : *params->markedIndices) {
                        arr[idx] = 0;
                    }
                    LeaveCriticalSection(params->arrayLock);
                    params->isFinished[id - 1] = true;
                    return 0;
                }
                break;
            }
        }
    }
}
