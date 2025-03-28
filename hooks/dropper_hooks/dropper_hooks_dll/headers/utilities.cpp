#pragma once
#include "pch.h"
#include "utilities.h"
#include "globals.h"

BOOL prepare_setup() {
    //create storage folder
    if (CreateDirectory(wstore_path.c_str(), NULL)) {
        //create file
        std::ofstream file(logFile, ios::app);
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
wstring string_towstring(string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}


map<string, string> process_filename(string original_filename) {
    std::map <string, string> fileinfo;
    hash<string> hash;

    size_t index = original_filename.find_last_of("\\");

    fileinfo["filename"] = original_filename.substr(index + 1);
    fileinfo["filepath"] = original_filename.substr(0, index);
    fileinfo["hash_path"] = to_string(hash(original_filename.substr(0, index)));
    size_t extension_index = fileinfo["filename"].find_last_of(".");
    fileinfo["file_Extension"] = fileinfo["filename"].substr(extension_index + 1);
    fileinfo["filename"] = fileinfo["filename"].substr(0, extension_index);


    return fileinfo;
}

string getTime() {
    time_t now = time(NULL);
    char tmBuff[30] = {};
    ctime_s(tmBuff, sizeof(tmBuff), &now);
    string timestamp = tmBuff;
    replace(timestamp.begin(), timestamp.end(), '\n', ','); //replace all \n to , 
    replace(timestamp.begin(), timestamp.end(), ' ', '_'); //replace all spaces to _ 
    return timestamp;
}

int call_childLogger(string log) {
    string sCmdLine = logger_path + " " + log;
    LPSTR  cmdline = &sCmdLine[0];


    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process. 
    if (!CreateProcessA(
        NULL,   // No module name (use command line)
        cmdline,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {


        return 0;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 1;


}

void saveBeforeDelete(LPCSTR lpfilename) {
    std::map <string, string> fileinfo = process_filename(lpfilename);
    string new_file = fileinfo["filename"] + "_" + fileinfo["hash_path"] + "." + fileinfo["file_Extension"];
    string complete_path = store_path + new_file;

    string log_line = getTime() + "DeleteFile" + "," + fileinfo["filepath"] + "," + fileinfo["filename"] + "." + fileinfo["file_Extension"] + "," + fileinfo["hash_path"];
    replace(log_line.begin(), log_line.end(), ' ', '_'); //replace all spaces to _
    call_childLogger(log_line);
    CopyFileA(lpfilename, complete_path.c_str(), FALSE);
}


