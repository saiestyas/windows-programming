// helloc2.cpp : This file contains the 'main' function. Program execution begins and ends there.
// simple tcp socket send example to send data to my lab c2

#include <iostream>
#include <vector>
#include <string.h>
// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

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
    send_packet(ip_address,port, chunk_size,message);

}

