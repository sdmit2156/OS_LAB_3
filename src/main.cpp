#include "marker.h"
#include <iostream>
#include <vector>
#include <windows.h>

int main() {
    int size;
    std::cout << "Enter array size: ";
    std::cin >> size;
    int* arr = new int[size]();
    int numThreads;
    std::cout << "Enter number of marker threads: ";
    std::cin >> numThreads;

    std::vector<HANDLE> threads(numThreads);
    std::vector<HANDLE> notifyEvents(numThreads);
    std::vector<MarkerParams> params(numThreads);
    std::vector<std::vector<int>> marked(numThreads);
    std::vector<HANDLE> terminateEvents(numThreads);
    std::vector<HANDLE> resumeEvents(numThreads);
    bool* isFinished = new bool[numThreads]();

    HANDLE startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    CRITICAL_SECTION arrayLock, consoleLock;
    InitializeCriticalSection(&arrayLock);
    InitializeCriticalSection(&consoleLock);

    for (int i = 0; i < numThreads; ++i) {
        int markerId = i + 1;
        notifyEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        resumeEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        terminateEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        params[i] = {
            markerId, arr, size, startEvent, resumeEvents[i], terminateEvents[i], notifyEvents[i],
            &arrayLock, &consoleLock, &marked[i], isFinished
        };
        threads[i] = CreateThread(NULL, 0, MarkerThread, &params[i], 0, NULL);
    }

    SetEvent(startEvent);

    int active = numThreads;
    while (active > 0) {
        std::vector<HANDLE> activeNotifyEvents;
        for (int i = 0; i < numThreads; ++i) {
            if (!isFinished[i]) {
                activeNotifyEvents.push_back(notifyEvents[i]);
            }
        }

        if (!activeNotifyEvents.empty()) {
            WaitForMultipleObjects(
                static_cast<DWORD>(activeNotifyEvents.size()),
                activeNotifyEvents.data(),
                TRUE,
                INFINITE
            );
        }

        EnterCriticalSection(&consoleLock);
        std::cout << "Array: ";
        for (int i = 0; i < size; ++i)
            std::cout << arr[i] << " ";
        std::cout << "\n";
        LeaveCriticalSection(&consoleLock);
        int toTerminate = -1;
        while (true) {
            std::cout << "Enter marker number to terminate (1-" << numThreads << "): ";
            std::cin >> toTerminate;
            toTerminate -= 1;

            if (toTerminate < 0 || toTerminate >= numThreads || isFinished[toTerminate]) {
                EnterCriticalSection(&consoleLock);
                std::cout << "Error: Marker " << (toTerminate + 1)
                    << " does not exist or is already terminated.\n";
                LeaveCriticalSection(&consoleLock);
            }
            else {
                break;
            }
        }

        SetEvent(terminateEvents[toTerminate]);
        WaitForSingleObject(threads[toTerminate], INFINITE);
        active--;

        EnterCriticalSection(&consoleLock);
        std::cout << "Array after terminating marker " << (toTerminate + 1) << ": ";
        for (int i = 0; i < size; ++i)
            std::cout << arr[i] << " ";
        std::cout << "\n";
        LeaveCriticalSection(&consoleLock);

        for (int i = 0; i < numThreads; ++i) {
            if (!isFinished[i] && i != toTerminate) {
                SetEvent(resumeEvents[i]);
            }
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
        CloseHandle(notifyEvents[i]);
        CloseHandle(terminateEvents[i]);
        CloseHandle(resumeEvents[i]);
    }
    CloseHandle(startEvent);
    DeleteCriticalSection(&arrayLock);
    DeleteCriticalSection(&consoleLock);
    delete[] arr;
    delete[] isFinished;

    std::cout << "All markers terminated.\n";
    return 0;
}
