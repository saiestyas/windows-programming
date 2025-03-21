// autohooks_createfile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <windows.h>
#include ".\headers\Release_MT_x64\MinHook.h"
#include <string>

using namespace std;

//return function declarations

//CreateFileA
using fnCreateFileA = HANDLE(WINAPI*)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
fnCreateFileA call_original_CreateFileA = NULL;

//CreateFileW
using fnCreateFileW = HANDLE(WINAPI*)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
fnCreateFileW call_original_CreateFileW = NULL;

//WriteFile
using fnWriteFile = BOOL(WINAPI*)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
fnWriteFile call_original_writefile = NULL;

//DeleteFileA
using fnDeleteFileA = BOOL(WINAPI*)(LPCSTR);
fnDeleteFileA call_original_deletefileA = NULL;

//DeleteFileW
using fnDeleteFileW = BOOL(WINAPI*)(LPCWSTR);
fnDeleteFileW call_original_deletefilew = NULL;

//hook functions - replacements
//CreateFileA
HANDLE WINAPI hookCreateFileA(LPCSTR filename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate) {
    // <placeholder>
    MessageBoxA(nullptr, "You are creating file!", "CreatefileA hook", MB_ICONINFORMATION);
    // </placeholder>
    return call_original_CreateFileA(filename, acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);
}

//CreateFileW
HANDLE WINAPI hookCreateFileW(LPCWSTR filename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate) {
    // <placeholder>
    MessageBoxA(nullptr, "You are creating file!", "CreatefileW hook", MB_ICONINFORMATION);
    // </placeholder>
    return call_original_CreateFileW(filename, acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);
}

//WriteFileEx
BOOL WINAPI  hookWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberofbytesWritten, LPOVERLAPPED lpOverlaped) {
    // <placeholder>
    MessageBoxA(nullptr, "You are writing file!", "WriteFile hook", MB_ICONINFORMATION);
    // </placeholder>
    return call_original_writefile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberofbytesWritten, lpOverlaped);
}

BOOL WINAPI hookDeleteFileA(LPCSTR lpfilename) {
    // <placeholder>
    MessageBoxA(nullptr, "You are deleting a file!", "DeleteFileA hook", MB_ICONINFORMATION);
    // </placeholder>
    return call_original_deletefileA(lpfilename);
}

BOOL WINAPI hookDeleteFileW(LPCWSTR lpfilename) {
    // <placeholder>
    MessageBoxA(nullptr, "You are deleting a file!", "DeleteFileW hook", MB_ICONINFORMATION);
    // </placeholder>
    return call_original_deletefilew(lpfilename);
}

//install the hooks
bool installHookCreateFileA() {
    MH_STATUS mhResult = MH_CreateHook(&CreateFileA, &hookCreateFileA, reinterpret_cast<LPVOID*>(&call_original_CreateFileA));

    if (mhResult != MH_OK) {
        cout << "CreateFileA HOOK: MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

    return TRUE;
}

bool installHookCreateFileW() {
    MH_STATUS mhResult = MH_CreateHook(&CreateFileW, &hookCreateFileW, reinterpret_cast<LPVOID*>(&call_original_CreateFileW));
    if (mhResult != MH_OK) {
        cout << "CreateFileW HOOK: MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

    return TRUE;
}

bool installHookWriteFile() {
    MH_STATUS mhResult = MH_CreateHook(&WriteFile, &hookWriteFile, reinterpret_cast<LPVOID*>(&call_original_writefile));
    if (mhResult != MH_OK) {
        cout << "WriteFile HOOK: MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

    return TRUE;
}

bool installHookDeleteFileA() {

    MH_STATUS mhResult = MH_CreateHook(&DeleteFileA, &hookDeleteFileA, reinterpret_cast<LPVOID*>(&call_original_deletefileA));
    if (mhResult != MH_OK) {
        cout << "DeleteFileA HOOK: MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

    return TRUE;
}

bool installHookDeleteFileW() {
    
        MH_STATUS mhResult = MH_CreateHook(&DeleteFileW, &hookDeleteFileW, reinterpret_cast<LPVOID*>(&call_original_deletefilew));
    if (mhResult != MH_OK) {
        cout << "DeleteFileW HOOK: MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

    return TRUE;
}

bool installHooks() {
    MH_STATUS mhResult = MH_Initialize();

    //initialize MH
    if (mhResult != MH_OK) {
        cout << "MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

    // install all hooks ~ disabled state
    installHookCreateFileA();
    installHookCreateFileW();
    installHookWriteFile();
    installHookDeleteFileA();
    installHookDeleteFileW();
    
   

    // Enable hooks
    if (MH_EnableHook(&CreateFileA) != MH_OK) {
        cout << "ENABLE CreateFileA HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;
        
    }

    if (MH_EnableHook(&CreateFileW) != MH_OK) {
        cout << "ENABLE CreateFileW HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;

    }

    if (MH_EnableHook(&WriteFile) != MH_OK) {
        cout << "ENABLE WriteFile HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;
        
    }


    if (MH_EnableHook(&DeleteFileA) != MH_OK) {
        cout << "ENABLE DeleteFileA HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;

    }

    if (MH_EnableHook(&DeleteFileW) != MH_OK) {
        cout << "ENABLE DeleteFileW HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;
        
    }

    return TRUE;
}

bool disableHooks() {
    MH_STATUS mhResult;
    
    mhResult = MH_DisableHook(&CreateFileA);
    if (mhResult != MH_OK) {
        cout << "[!] Disabling CreateFileA HOOK: MH_DisabbleHook Failed with error: " << mhResult << endl;
    }
    else {
        mhResult = MH_RemoveHook(&CreateFileA);
        if (mhResult != MH_OK) {
            cout << "[!] Removing CreateFileA HOOK: MH_RemoveHook Failed with error: " << mhResult << endl;
        }
    }
    

    mhResult = MH_DisableHook(&CreateFileW);
    if (mhResult != MH_OK) {
        cout << "[!] Disabling CreateFileW HOOK: MH_DisabbleHook Failed with error: " << mhResult << endl;
        return FALSE;
    }
    else {
        mhResult = MH_RemoveHook(&CreateFileW);
        if (mhResult != MH_OK) {
            cout << "[!] Disabling CreateFileW HOOK: MH_RemoveHook Failed with error: " << mhResult << endl;
        }
    }
      

    mhResult = MH_DisableHook(&WriteFile);
    if (mhResult != MH_OK) {
        cout << "[!] Disabling WriteFile HOOK: MH_DisabbleHook Failed with error: " << mhResult << endl;
    }
    else {
        mhResult = MH_RemoveHook(&WriteFile);
        if (mhResult != MH_OK) {
            cout << "[!] Disabling WriteFile HOOK: MH_RemoveHook Failed with error: " << mhResult << endl;
        }
    }

    
    mhResult = MH_DisableHook(&DeleteFileW);
    if (mhResult != MH_OK) {
        cout << "[!] Disabling DeleteFileW HOOK: MH_DisabbleHook Failed with error: " << mhResult << endl;
    }
    else {
        mhResult = MH_RemoveHook(&DeleteFileW);
        if (mhResult != MH_OK) {
            cout << "[!] Disabling DeleteFileW HOOK: MH_RemoveHook Failed with error: " << mhResult << endl;
        }
    }

    

    return TRUE;
}

//Related with the test programm
void CALLBACK WriteCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode == 0) {
        cout << "[*] WriteFileEx completed successfully. Bytes written: " << dwNumberOfBytesTransfered << endl;
    }
    else {
        cerr << "[!] WriteFileEx failed with error code: " << dwErrorCode << endl;
    }
}

bool createfileW_writeFile() {
    HANDLE h1 = CreateFileW(
        L"C:\\ProgramData\\test1.bin",
        (GENERIC_READ | GENERIC_WRITE),
        NULL,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h1 == INVALID_HANDLE_VALUE) {
        cerr << "[!] Error in C:\\ProgramData\\test1.bin file creation, error code:" << GetLastError() << endl;
        return FALSE;
    }
    else {
        cout << "[*] C:\\ProgramData\\test1.bin created." << endl;
        const wchar_t* buffer = L"this is the first test message";
        DWORD writenbytes;
        Sleep(1000);
        cout << "[*] Writing into file 1" << endl;
        WriteFile(h1,
            buffer,
            (DWORD)wcslen(buffer) * sizeof(wchar_t),
            &writenbytes,
            NULL);

        CloseHandle(h1);
        return TRUE;
    }
}

bool createfileA_WriteFileEx() {

    HANDLE h2 = CreateFileA(
        "C:\\Users\\Public\\test2.bin",
        (GENERIC_READ | GENERIC_WRITE),
        NULL,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );


    if (h2 == INVALID_HANDLE_VALUE) {
        cerr << "[!] Error in C:\\Users\\Public\\test2.bin file creation, error code:" << GetLastError() << endl;
        return FALSE;
    }
    else {
        cout << "[*] C:\\Users\\Public\\test2.bin created." << endl;
        OVERLAPPED overlap = { 0 };
        const wchar_t* buffer = L"this is the second test message";
        Sleep(1000);
        cout << "[*] Writing into file 2" << endl;
        WriteFileEx(h2,
            buffer,
            (DWORD)wcslen(buffer) * sizeof(wchar_t),
            &overlap,
            WriteCompletionRoutine);

        CloseHandle(h2);
        return TRUE;
    }
}

bool createOffstreamFile() {
    std::ofstream file("C:\\Users\\Public\\test3.bin");

    if (!file) {
        cout << "[!] Error in C:\\Users\\Public\\test3.bin file creation" << endl;
        return FALSE;
    }

    //lets write some text
    cout << "[*] C:\\Users\\Public\\test3.bin created." << endl;
    Sleep(1000);
    cout << "[*] Writing into file 3" << endl;
    file << "this is the third test" << endl;
    file.close();
    return TRUE;
}

void deletefiles() {
    if (DeleteFileW(L"C:\\ProgramData\\test1.bin")) {
        cout << "[*] C:\\ProgramData\\test1.bin was deleted" << endl;
    }
    else {
        cout << "[!] C:\\ProgramData\\test1.bin deletion error code:" << GetLastError() << endl;
    }
    if (DeleteFileA("C:\\Users\\Public\\test2.bin")) {
        cout << "[*] C:\\Users\\Public\\test2.bin was deleted" << endl;
    }
    else {
        cout << "[!] C:\\Users\\Public\\test2.bin deletion error code:" << GetLastError() << endl;
    }

    if (DeleteFileA("C:\\Users\\Public\\test3.bin")) {
        cout << "[*] C:\\Users\\Public\\test3.bin was deleted" << endl;
    }
    else {
        cout << "[!] C:\\Users\\Public\\test3.bin deletion error code:" << GetLastError() << endl;
    }

}

int main() {

    installHooks();
    createfileW_writeFile();
    createfileA_WriteFileEx();
    createOffstreamFile();
    Sleep(5000);
    deletefiles();
    disableHooks();
    return 0;


}