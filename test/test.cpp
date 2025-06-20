#include "marker.h"
#include "catch_amalgamated.hpp"
#include <windows.h>
#include <vector>

TEST_CASE("Marker thread marks array correctly", "[marker]") {
    int size = 10;
    int* arr = new int[size]();
    CRITICAL_SECTION arrayLock, consoleLock;
    InitializeCriticalSection(&arrayLock);
    InitializeCriticalSection(&consoleLock);
    std::vector<int> markedIndices;
    bool isFinished = false;

    HANDLE startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE resumeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE terminateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE notifyMain = CreateEvent(NULL, FALSE, FALSE, NULL);

    MarkerParams params = {
        1, arr, size, startEvent, resumeEvent, terminateEvent, notifyMain,
        &arrayLock, &consoleLock, &markedIndices, &isFinished
    };

    HANDLE thread = CreateThread(NULL, 0, MarkerThread, &params, 0, NULL);
    SetEvent(startEvent);

    WaitForSingleObject(notifyMain, INFINITE);

    bool marked = false;
    for (int i = 0; i < size; ++i) {
        if (arr[i] == 1) {
            marked = true;
            break;
        }
    }
    REQUIRE(marked == true);

    SetEvent(terminateEvent);
    WaitForSingleObject(thread, INFINITE);

    bool cleared = true;
    for (int i = 0; i < size; ++i) {
        if (arr[i] != 0) {
            cleared = false;
            break;
        }
    }
    REQUIRE(cleared == true);

    DeleteCriticalSection(&arrayLock);
    DeleteCriticalSection(&consoleLock);
    CloseHandle(startEvent);
    CloseHandle(resumeEvent);
    CloseHandle(terminateEvent);
    CloseHandle(notifyMain);
    CloseHandle(thread);
    delete[] arr;
}

TEST_CASE("Marker thread handles termination correctly", "[marker]") {
    int size = 10;
    int* arr = new int[size]();
    CRITICAL_SECTION arrayLock, consoleLock;
    InitializeCriticalSection(&arrayLock);
    InitializeCriticalSection(&consoleLock);
    std::vector<int> markedIndices;
    bool isFinished = false;

    HANDLE startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE resumeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE terminateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE notifyMain = CreateEvent(NULL, FALSE, FALSE, NULL);

    MarkerParams params = {
        1, arr, size, startEvent, resumeEvent, terminateEvent, notifyMain,
        &arrayLock, &consoleLock, &markedIndices, &isFinished
    };

    HANDLE thread = CreateThread(NULL, 0, MarkerThread, &params, 0, NULL);
    SetEvent(startEvent);

    SetEvent(terminateEvent);
    WaitForSingleObject(thread, INFINITE);

    REQUIRE(isFinished == true);

    DeleteCriticalSection(&arrayLock);
    DeleteCriticalSection(&consoleLock);
    CloseHandle(startEvent);
    CloseHandle(resumeEvent);
    CloseHandle(terminateEvent);
    CloseHandle(notifyMain);
    CloseHandle(thread);
    delete[] arr;
}