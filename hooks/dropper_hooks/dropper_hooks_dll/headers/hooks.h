#pragma once
#include "headers.h"


//hook replacements
HANDLE WINAPI hookCreateFileA(LPCSTR ifilename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate);
HANDLE WINAPI hookCreateFileW(LPCWSTR ifilename, DWORD acessmode, DWORD sharemode, LPSECURITY_ATTRIBUTES secattrb, DWORD creationdisp, DWORD flagsNattributes, HANDLE htemplate);
BOOL   WINAPI hookWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberofbytesWritten, LPOVERLAPPED lpOverlaped);
BOOL   WINAPI hookCloseHandle(HANDLE hObject);
BOOL   WINAPI hookDeleteFileA(LPCSTR lpfilename);
BOOL   WINAPI hookDeleteFileW(LPCWSTR lpfilename);

//INSTALL HOOKS
bool installHookCreateFileA();
bool installHookCreateFileW();
bool installHookWriteFile();
bool installHookDeleteFileA();
bool installHookDeleteFileW();
bool installHooks();

//DISABLE HOOKS
bool disableHooks();