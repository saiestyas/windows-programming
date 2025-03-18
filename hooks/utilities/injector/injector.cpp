// injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <assert.h>

using namespace std;

PROCESS_INFORMATION createSuspendedProcess(const char* exePath) {
 
   
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
 
    // Crear proceso en estado SUSPENDIDO
    if (CreateProcessA(exePath,   // Ruta del ejecutable
                       NULL,       // Parámetros de línea de comandos
                       NULL,       // Atributos de seguridad para el proceso
                       NULL,       // Atributos de seguridad para el hilo
                       FALSE,      // Heredar handles
                       CREATE_SUSPENDED, // El proceso queda en estado suspendido
                       NULL,       // Variables de entorno
                       NULL,       // Directorio de trabajo
                       &si,        // Información de inicio
                       &pi))       // Información del proceso
    {
        std::wcout << L"Process was created on suspended mode: PID " << pi.dwProcessId << std::endl;
 
        return pi;
 
    }
    else {
        std::cerr << "Error creating new process: " << GetLastError() << std::endl;
    }
 
}
    

bool inject_dll(const char* dll_path, PROCESS_INFORMATION pi) {
    //obtain handle with required permissions
    HANDLE hProcess = OpenProcess((PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD), FALSE, pi.dwProcessId);
    if (!hProcess) {
        cout << "[!] Handle could not be obtained, error code: " << GetLastError() << endl;
        return 1;
    }

    cout << "[*] Process handle obtained successfully" << endl;

    auto p = VirtualAllocEx(hProcess, nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(p);

    WriteProcessMemory(hProcess, p, dll_path, strlen(dll_path) + 1, nullptr);

    auto hThread = CreateRemoteThread(hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle(L"kernel32.dll"), "LoadLibraryA"),
        p, 0, nullptr);
    assert(hThread);


    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    cout << "[*] Injection was executed successfully" << endl;
    return 0;

    
}

int main(int argc, const char* argv[]) {
    if (argc < 3) {
        cout << "[!] Usage: injector.exe <suspicious.exe> <dllpath>" << endl << "[!] Please use complete directory pathss" << endl;
        return 0;
    }

    PROCESS_INFORMATION pi = createSuspendedProcess(argv[1]);
    inject_dll(argv[2],pi);
    // Restart process execution
    ResumeThread(pi.hThread);
    // Close handles
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

}

