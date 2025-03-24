// autohooks_createfile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include <iostream>
#include <fstream>
#include <windows.h>
#include ".\headers\Release_MT_x64\MinHook.h"
#include <string>
#include <ctime>

using namespace std;
string  store_path  = "C:\\ProgramData\\drooper_hooks\\";
wstring wstore_path = L"C:\\ProgramData\\drooper_hooks\\";
string logFile = store_path + "dropper.logs";

static bool isHookingInProgress = false;

//hook funcionality
//start this before hook installition
BOOL prepare_setup() {
    //create storage folder
    if (CreateDirectory(wstore_path.c_str(), NULL)) {
        //create file
        std::ofstream file(logFile,  ios::app);
        if (!file) {
            cout << "[!] Error in C:\\ProgramData\\drooper_hooks\\drooper.logs file creation" << endl;
            return FALSE;
        }
        file << "TimeStamp, Operation, Original_FilePath, Original_Filename, HASH(FilePath), Local_Filename" << endl;
        file.close();
        return TRUE;
    }
    else {
        cout << "[!] Error in C:\\ProgramData\\drooper_hooks folder creation" << endl;
        return FALSE;
    }

}

string wstring_tostring(wstring ws) {
    
    string str(ws.begin(), ws.end());
    return str;
}

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
HANDLE WINAPI hookCreateFileA(LPCSTR ifilename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate) {
    if (isHookingInProgress) {
        return call_original_CreateFileA(ifilename, acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);
    }
    isHookingInProgress = true;
    
   //save the original_file name:: path + filename
    string original_filename = ifilename;
    //obtain filename
    int last_index = original_filename.find_last_of("\\");
    string filename = original_filename.substr(last_index + 1, strlen(ifilename) - 1);
    //obtain filepath
    string filepath = original_filename.substr(0, last_index);
    //obtain a simple hash value
    hash<string> hash;
    string hash_str = to_string(hash(filename));
    //create the copy_filename
    int extension_index = filename.find_last_of(".");
    string cpy_filename = filename.substr(0, extension_index) + "_" + hash_str +"." + filename.substr(extension_index + 1); //file it's going to be created in our drooper_hooks folder ==> filename_hash.extension
    
    std::ofstream log_file(logFile, ios::app);
    //log results:: TimeStamp, Operation, Original_FilePath, Original_Filename, HASH(FilePath), Local_Filename
    time_t now = time(NULL);
    char tmBuff[30] = {};
    ctime_s(tmBuff, sizeof(tmBuff), &now);
    string timestamp = tmBuff;
    size_t slash_n = timestamp.find("\n");
    log_file << timestamp.substr(0,slash_n) << "," << "FileCreation-A" << "," << filepath << "," << filename << "," << hash_str << "," << cpy_filename << endl;
    log_file.close();
    
    //save a copy
    string complete_path = store_path + cpy_filename;
    HANDLE hcopia = call_original_CreateFileA(complete_path.c_str(), acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);
    isHookingInProgress = false;
    return hcopia;
}

//CreateFileW
HANDLE WINAPI hookCreateFileW(LPCWSTR ifilename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate) {
 

    if (isHookingInProgress) {
        return call_original_CreateFileW(ifilename, acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);
    }
    isHookingInProgress = true;

    //save the original_file name:: path + filename
    wstring original_filename = ifilename;
    //obtain filename
    int last_index = original_filename.find_last_of(L"\\");
    wstring filename = original_filename.substr(last_index + 1, wcslen(ifilename) - 1);
    //obtain filepath
    wstring filepath = original_filename.substr(0, last_index);
    //obtain a simple hash value
    hash<wstring> hash;
    wstring hash_str = to_wstring(hash(filename));
    //create the copy_filename
    int extension_index = filename.find_last_of(L".");
    wstring cpy_filename = filename.substr(0, extension_index) + L"_" + hash_str + L"." + filename.substr(extension_index + 1); //file it's going to be created in our drooper_hooks folder ==> filename_hash.extension
    
    
    std::ofstream log_file(logFile, ios::app);
        
    time_t now = time(NULL);
    char tmBuff[30] = {};
    ctime_s(tmBuff, sizeof(tmBuff), &now);
    string timestamp = tmBuff;
    size_t slash_n = timestamp.find("\n");

    //log results:: TimeStamp, Operation, Original_FilePath, Original_Filename, HASH(FilePath), Local_Filename
    log_file << timestamp.substr(0, slash_n) << "," << "FileCreation-W" << "," << wstring_tostring(filepath) << "," << wstring_tostring(filename)<< "," << wstring_tostring(hash_str) << "," << wstring_tostring(cpy_filename) << endl;
    log_file.close();

    //save a copy
    wstring complete_path = wstore_path + cpy_filename;
    HANDLE hcopia=call_original_CreateFileW(complete_path.c_str(), acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);
    isHookingInProgress = false;
    return hcopia;
    
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

    if (!prepare_setup()) {
        return -1;
    }

    installHooks();
    createfileW_writeFile();
    createfileA_WriteFileEx();
    /*
    createOffstreamFile();
    Sleep(5000);
    deletefiles();
    */
    disableHooks();
    return 0;


}