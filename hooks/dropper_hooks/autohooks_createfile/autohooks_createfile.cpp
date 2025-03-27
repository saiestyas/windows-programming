// autohooks_createfile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "headers.h"
#include "utilities.h"
#include "globals.h"
#include "hooks.h"
using namespace std;



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

    if (!prepare_setup()) {
        return -1;
    }

    installHooks();
    createfileW_writeFile();
    createfileA_WriteFileEx();
    createOffstreamFile();
    Sleep(5000);
    deletefiles();
    disableHooks();
    return 0;


}