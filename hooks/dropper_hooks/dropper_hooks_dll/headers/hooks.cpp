#include "pch.h"
#include "utilities.h"
#include "globals.h"

//JMP Definition
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

//CloseHandle
using fnCloseHandle = BOOL(WINAPI*)(HANDLE hObject);
fnCloseHandle call_original_CloseHandle = NULL;

//hook functions - replacements

HANDLE WINAPI hookCreateFileA(LPCSTR ifilename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate) {

    // ProcessFileName fileinfo["filename"], fileinfo["filepath"], fileinfo["hash_path"], fileinfo["file_Extension"]
    std::map <string, string> fileinfo = process_filename((string)ifilename);
    string copy_filename = fileinfo["filename"] + "_" + fileinfo["hash_path"] + "." + fileinfo["file_Extension"];

    //log situation
    string log_line = getTime() + "CreateFile" + "," + fileinfo["filepath"] + "," + fileinfo["filename"] + "." + fileinfo["file_Extension"] + "," + fileinfo["hash_path"] + "," + copy_filename;
    replace(log_line.begin(), log_line.end(), ' ', '_'); //replace all spaces to _
    call_childLogger(log_line);

    //save a copy
    string complete_path = store_path + copy_filename;
    HANDLE hcopia = call_original_CreateFileA(complete_path.c_str(), acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);

    if (hcopia != INVALID_HANDLE_VALUE)
        CloseHandle(hcopia);

    return call_original_CreateFileA(ifilename, acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);;
}


HANDLE WINAPI hookCreateFileW(LPCWSTR ifilename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate) {

    // ProcessFileName fileinfo["filename"], fileinfo["filepath"], fileinfo["hash_path"], fileinfo["file_Extension"]
    std::map <string, string> fileinfo = process_filename(wstring_tostring(ifilename));
    string copy_filename = fileinfo["filename"] + "_" + fileinfo["hash_path"] + "." + fileinfo["file_Extension"];

    string log_line = getTime() + "CreateFile" + "," + fileinfo["filepath"] + "," + fileinfo["filename"] + "." + fileinfo["file_Extension"] + "," + fileinfo["hash_path"] + "," + copy_filename;
    replace(log_line.begin(), log_line.end(), ' ', '_'); //replace all spaces to _
    call_childLogger(log_line);

    //save a copy
    string complete_path = store_path + copy_filename;
    wstring wcomplete_path = string_towstring(complete_path);
    HANDLE hcopia = call_original_CreateFileW(wcomplete_path.c_str(), acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);

    if (hcopia != INVALID_HANDLE_VALUE)
        CloseHandle(hcopia);

    return call_original_CreateFileW(ifilename, acessmode, sharemode, secattrb, creationdisp, flagsNattributes, htemplate);

}


BOOL WINAPI  hookWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberofbytesWritten, LPOVERLAPPED lpOverlaped) {

    //write log 
    if (hFile != INVALID_HANDLE_VALUE) {
        char filePath[MAX_PATH];
        if (GetFinalPathNameByHandleA(hFile, filePath, MAX_PATH, FILE_NAME_NORMALIZED)) {
            BOOL result = call_original_writefile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberofbytesWritten, lpOverlaped);
            //log situation
            string log_line = getTime() + "WriteFile," + filePath;
            replace(log_line.begin(), log_line.end(), ' ', '_'); //replace all spaces to _
            call_childLogger(log_line);
            return result;
        }
    }

    //log situation
    string log_line = getTime() + "WriteFile,UNKNOWN";
    call_childLogger(log_line);

    return call_original_writefile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberofbytesWritten, lpOverlaped);

}


BOOL WINAPI hookCloseHandle(HANDLE hObject) {
    char filePath[MAX_PATH];

    if (hObject != INVALID_HANDLE_VALUE) {
        if (GetFileType(hObject) == FILE_TYPE_DISK) {
            if (GetFinalPathNameByHandleA(hObject, filePath, MAX_PATH, FILE_NAME_NORMALIZED)) {
                bool result = call_original_CloseHandle(hObject);
                if (result != 0) { //if the hObject was released, get a file copy 
                    std::map <string, string> fileinfo = process_filename(filePath);
                    unsigned int seed;
                    rand_s(&seed);
                    string filename = store_path + fileinfo["filename"] + "_" + to_string(seed) + ".tmp";
                    CopyFileA(filePath, filename.c_str(), FALSE);
                }
                //in case of bad handle closing, return
                return result;
            }
        }
    }
    return call_original_CloseHandle(hObject);
}

BOOL WINAPI hookDeleteFileA(LPCSTR lpfilename) {
    saveBeforeDelete(lpfilename);

    return call_original_deletefileA(lpfilename);
}

BOOL WINAPI hookDeleteFileW(LPCWSTR lpfilename) {
    string sfilename = wstring_tostring(lpfilename);
    saveBeforeDelete(sfilename.c_str());


    return call_original_deletefilew(lpfilename);
}



//INSTALL HOOKS
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

bool installHookCloseHandle() {
    MH_STATUS mhResult = MH_CreateHook(&CloseHandle, &hookCloseHandle, reinterpret_cast<LPVOID*>(&call_original_CloseHandle));
    if (mhResult != MH_OK) {
        cout << "WriteFile HOOK: MH_Initialize Failed with error: " << mhResult << endl;
        return FALSE;
    }

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
    installHookCloseHandle();
    installHookDeleteFileA();
    installHookDeleteFileW();



    // ENABLE HOOKS

    if (MH_EnableHook(&CreateFileA) != MH_OK) {
        cout << "ENABLE CreateFileA HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;
    }

    if (MH_EnableHook(&CreateFileW) != MH_OK) {
        cout << "ENABLE CreateFileW HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;

    }

    if (MH_EnableHook(&WriteFile) != MH_OK) {
        cout << "ENABLE WriteFile HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;

    }

    if (MH_EnableHook(&CloseHandle) != MH_OK) {
        cout << "ENABLE CloseHandle HOOK: MH_EnableHook Failed with error: " << GetLastError() << endl;
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

    mhResult = MH_DisableHook(&CloseHandle);
    if (mhResult != MH_OK) {
        cout << "[!] Disabling CloseHandle HOOK: MH_DisabbleHook Failed with error: " << mhResult << endl;
    }
    else {
        mhResult = MH_RemoveHook(&CloseHandle);
        if (mhResult != MH_OK) {
            cout << "[!] Removing CloseHandle HOOK: MH_RemoveHook Failed with error: " << mhResult << endl;
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