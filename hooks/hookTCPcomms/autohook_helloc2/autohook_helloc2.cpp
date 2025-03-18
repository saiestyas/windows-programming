// helloc2.cpp : This file contains the 'main' function. Program execution begins and ends there.
// minhook poc to test hooking multiple functions in a local process

#include <iostream>
#include <vector>
#include <string.h>
// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <MinHook.h>

using namespace std;

//RETURN POINTERS
using psend = int (WINAPI *)(SOCKET, const char*, int, int);
psend call_original_send = NULL;

using psocket = SOCKET (WINAPI*)(int,int,int);
psocket call_origonal_socket = NULL;

using pconnect = int(WINAPI*)(SOCKET, const sockaddr *, int);
pconnect call_original_connect = NULL;

using pcloseSocket = int(WINAPI*)(SOCKET);
pcloseSocket call_original_closesocket = NULL;

//REPLACEMENTS
INT WINAPI send_replacement(SOCKET s, const char* buf, int len, int flags) {
    cout << "[*] Socket:" << s << " payload message: " << buf << endl;
    cout << buf << endl;
    return call_original_send(s,buf,len,flags);
}

SOCKET WINAPI socket_replacement(int af, int type, int protocol) {
    cout << "[*] Socket creation intercepted, address family:" << af << " protocol: " << protocol << endl;
    return call_origonal_socket(af, type, protocol);
}

INT WINAPI connect_replacement(SOCKET s, const sockaddr *name, int namelen) {
    char ipv4[INET_ADDRSTRLEN]  = { 0 };
    char ipv6[INET6_ADDRSTRLEN] = { 0 };

    if (name->sa_family == AF_INET) { //ipv4
        sockaddr_in* addr_in = (sockaddr_in*)name;
        inet_ntop(AF_INET, &(addr_in->sin_addr), ipv4, INET_ADDRSTRLEN);
        cout << "[*] Socket:" << s << " Connection stablishment. IPadress_(IPv4):" << ipv4 << " to port:" << ntohs(addr_in->sin_port) << endl;
    }

    if (name->sa_family == AF_INET6) { //ipv6
        sockaddr_in6* addr_in6 = (sockaddr_in6*)name;
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ipv6, INET6_ADDRSTRLEN);
        cout << "[*] Socket:" << s << " Connection stablishment. IPadress_(IPv6):" << ipv6 << " to port:" << ntohs(addr_in6->sin6_port) << endl;
    }
    
    return call_original_connect(s, name,namelen);
}

INT closesocket_replacement(SOCKET s) {
    cout << "[*] Socket: " << s << " has been closed" << endl;
    return call_original_closesocket(s);
}

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
    DWORD send_err     = MH_EnableHook(&send);
    DWORD connect_err  = MH_EnableHook(&connect);
    DWORD socket_err   = MH_EnableHook(&socket);
    DWORD closeSock_err= MH_EnableHook(&closesocket);

    if (send_err != MH_OK ) {
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
    send_err = connect_err = socket_err = closesocket =  NULL;

    //disable hooks
    send_err    = MH_DisableHook(&send);
    connect_err = MH_DisableHook(&connect);
    socket_err  = MH_DisableHook(&socket);
    socket_err  = MH_DisableHook(&socket);
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

void send_packet(char* ip_address, char* port, int chunk_size, vector<char*> payload) {

    WSADATA wsaData = { 0 };
    struct addrinfo* result = NULL,
        hints;

    //Initialize WSAStartup
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //DEFINE SOCKET COMMUNICATIONS: IPV4 TCP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //Resolve server address and port.
    if (getaddrinfo(ip_address, port, &hints, &result) != 0) {
        wcout << "getaddressinfo error" << GetLastError() << endl;
    }

    //connection attemp
    SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    int    err = connect(sock, result->ai_addr, (int)result->ai_addrlen);
    
    if (err == SOCKET_ERROR) {
        wcout << L"Connection error" << endl;
    }
    else {
        wcout << L"Connection success" << endl;
    }

    //Send buffer
    int i = 0;
    for (const auto& data : payload) {
        //cout << data;
        wcout << "Send packet " << i << L": " << chunk_size << L" bytes" << endl;
        send(sock, data, chunk_size, 0);
        Sleep(250); //necessary to let process
        i++;
    }

    WSACleanup();
    closesocket(sock);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <ip_address> <port>\n";
        return 1;
    }
    char* ip_address = argv[1];
    char* port = argv[2];
    DWORD chunk_size = 32;

    char m1[] = "This is a test message";
    char m2[] = "This is second test message";
    vector<char*> message = { m1,m2 };

    cout << "press intro to send messages" << endl;
    getchar();

    cout << "Enabling hooks" << endl;

    //Hooking
    if (!installHooks()) {
        return -1;
    }

    //test
    send_packet(ip_address, port, chunk_size, message);

    cout << "Disabling hooks" << endl;

    //Unhook
    if (!unHook()) {
        return -1;
    }
    
    return 0;

}

