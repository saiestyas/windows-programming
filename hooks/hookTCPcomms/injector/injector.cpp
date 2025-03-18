// injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <assert.h>
using namespace std;
int main(int argc, const char* argv[]) {
    if (argc < 3) {
        cout << "[!] Usage: injector.exe <PID> <dllpath>" << endl;
        return 0;
    }

    auto pid = atoi(argv[1]);

    HANDLE hProcess = OpenProcess((PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD), FALSE, pid);
    if (!hProcess) {
        cout << "[!] Handle could not be obtained, error code: " <<GetLastError() <<  endl;
        return 1;
    }

    cout << "[*] Process handle obtained successfully" << endl;

    auto p = VirtualAllocEx(hProcess, nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(p);

    WriteProcessMemory(hProcess, p, argv[2], strlen(argv[2]) + 1, nullptr);

    auto hThread = CreateRemoteThread(hProcess, nullptr, 0,
    (LPTHREAD_START_ROUTINE)GetProcAddress(
                                            GetModuleHandle(L"kernel32.dll"), "LoadLibraryA"),
                                            p,0,nullptr);
    assert(hThread);

    

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    cout << "[*] Injection was executed successfully" << endl;
    return 0;
}


