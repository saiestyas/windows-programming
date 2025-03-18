// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>

#include <MinHook.h>
#include <fstream>
#include <string>
using namespace std;

//RETURN POINTERS
using psend = int (WINAPI*)(SOCKET, const char*, int, int);
psend call_original_send = NULL;

using psocket = SOCKET(WINAPI*)(int, int, int);
psocket call_origonal_socket = NULL;

using pconnect = int(WINAPI*)(SOCKET, const sockaddr*, int);
pconnect call_original_connect = NULL;

using pcloseSocket = int(WINAPI*)(SOCKET);
pcloseSocket call_original_closesocket = NULL;

//Utilities
void logger(string error_line) {

    ofstream file;
    file.open("C:\\ProgramData\\commInject.log", ios::app); //append mode

    try {
        // Unable to open file, force exception
        if (!file.is_open()) {
            throw runtime_error("Error opening file: commInject.log");
        }
        //File successfully open
        file << error_line << endl;
        file.close();
    }
    catch (const runtime_error& e) {
        cout << "Error, log unable to be captured: " << e.what() << endl;
    }
}

//REPLACEMENTS
INT WINAPI send_replacement(SOCKET s, const char* buf, int len, int flags) {
    logger("[*] Socket:" + to_string(s) + " payload message: "+ buf);   
    return call_original_send(s, buf, len, flags);
}

SOCKET WINAPI socket_replacement(int af, int type, int protocol) {
    logger("[*] Socket creation intercepted, address family : " + to_string(af) + " protocol: " + to_string(protocol));
    return call_origonal_socket(af, type, protocol);
}

INT WINAPI connect_replacement(SOCKET s, const sockaddr* name, int namelen) {
    char ipv4[INET_ADDRSTRLEN] = { 0 };
    char ipv6[INET6_ADDRSTRLEN] = { 0 };

    if (name->sa_family == AF_INET) { //ipv4
        sockaddr_in* addr_in = (sockaddr_in*)name;
        inet_ntop(AF_INET, &(addr_in->sin_addr), ipv4, INET_ADDRSTRLEN);
        logger("[*] Socket:" + to_string(s) + " Connection stablishment. IPadress_(IPv4):" + ipv4 + " to port:" + to_string(ntohs(addr_in->sin_port)));
    
    }

    if (name->sa_family == AF_INET6) { //ipv6
        sockaddr_in6* addr_in6 = (sockaddr_in6*)name;
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ipv6, INET6_ADDRSTRLEN);
        logger("[*] Socket:" + to_string(s) + " Connection stablishment. IPadress_(IPv6):" + ipv6 + " to port:" + to_string(ntohs(addr_in6->sin6_port)));
    }

    return call_original_connect(s, name, namelen);
}

INT closesocket_replacement(SOCKET s) {
    logger("[*] Socket:" + to_string(s) + " has been closed");
    return call_original_closesocket(s);
}

//hook creations - single hooking
bool send_hook() {
    //send hook
    DWORD dwError = MH_CreateHook(&send, &send_replacement, reinterpret_cast<LPVOID*>(&call_original_send));
    if (dwError != MH_OK) {
        cout << " SEND HOOK: MH_Initialize Failed with error: " << dwError;
        return FALSE;
    }

    return TRUE;
}

bool socket_hook() {

    //socket hook
    DWORD dwError = MH_CreateHook(&socket, &socket_replacement, reinterpret_cast<LPVOID*>(&call_origonal_socket));
    if (dwError != MH_OK) {
        cout << "SOCKET HOOK: MH_Initialize Failed with error: " << dwError;
        return FALSE;
    }

    return TRUE;
}

bool connect_hook() {

    //connect hook
    DWORD dwError = MH_CreateHook(&connect, &connect_replacement, reinterpret_cast<LPVOID*>(&call_original_connect));
    if (dwError != MH_OK) {
        cout << " CONNECT HOOK: MH_Initialize Failed with error: " << dwError;
        return FALSE;
    }

    return TRUE;
}

bool close_socket() {
    //closesocket hook
    DWORD dwError = MH_CreateHook(&closesocket, &closesocket_replacement, reinterpret_cast<LPVOID*>(&call_original_closesocket));
    if (dwError != MH_OK) {
        cout << " CLOSE HOOK: MH_Initialize Failed with error: " << dwError;
        return FALSE;
    }

    return TRUE;
}

bool installHooks() {
    DWORD dwError = NULL;

    //initialize MH
    dwError = MH_Initialize();
    if (dwError != MH_OK) {
        cout << "MH_Initialize Failed with error: " << dwError;
        return FALSE;
    }

    //Create hooks
    if (!send_hook()) {
        return FALSE;
    }

    if (!socket_hook()) {
        return FALSE;
    }

    if (!connect_hook()) {
        return FALSE;
    }

    if (!close_socket()) {
        return FALSE;
    }



    //Enable hooks
    DWORD send_err      = MH_EnableHook(&send);
    DWORD connect_err   = MH_EnableHook(&connect);
    DWORD socket_err    = MH_EnableHook(&socket);
    DWORD closeSock_err = MH_EnableHook(&closesocket);

    if (send_err != MH_OK) {
        cout << "ENABLE SEND HOOK: MH_EnableHook Failed with error: " << send_err << endl;
        return FALSE;
    }
    else if (connect_err != MH_OK) {
        cout << "ENABLE CONNECT HOOK: MH_EnableHook Failed with error: " << connect_err << endl;
        return FALSE;
    }

    else if (socket_err != MH_OK) {
        cout << "ENABLE SOCKET HOOK: MH_EnableHook Failed with error: " << socket_err << endl;
        return FALSE;
    }

    else if (closeSock_err != MH_OK) {
        cout << "ENABLE Closesocket HOOK: MH_EnableHook Failed with error: " << socket_err << endl;
        return FALSE;
    }


    return TRUE;
}

bool unHook() {
    DWORD send_err, connect_err, socket_err, closesocket;
    send_err = connect_err = socket_err = closesocket = NULL;

    //disable hooks
    send_err =    MH_DisableHook(&send);
    connect_err = MH_DisableHook(&connect);
    socket_err =  MH_DisableHook(&socket);
    socket_err =  MH_DisableHook(&socket);
    closesocket = MH_DisableHook(&closesocket);

    if (send_err != MH_OK) {
        cout << "ENABLE SEND HOOK: MH_EnableHook Failed with error: " << send_err << endl;
        return FALSE;
    }
    else if (connect_err != MH_OK) {
        cout << "ENABLE CONNECT HOOK: MH_EnableHook Failed with error: " << connect_err << endl;
        return FALSE;
    }

    else if (socket_err != MH_OK) {
        cout << "ENABLE SOCKET HOOK: MH_EnableHook Failed with error: " << socket_err << endl;
        return FALSE;
    }

    else if (closesocket != MH_OK) {
        cout << "ENABLE SOCKET HOOK: MH_EnableHook Failed with error: " << socket_err << endl;
        return FALSE;
    }

    //Clean MH structure
    DWORD dwError = MH_Uninitialize();
    if (dwError != MH_OK) {
        cout << "MH_Uninitialize Failed with error: " << dwError;
        return FALSE;
    }

    return TRUE;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        return installHooks();
        break;
    }
    return TRUE;
}

