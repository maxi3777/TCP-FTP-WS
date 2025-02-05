#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Tchar.h>
#include "ServerFunctions.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define DEFAULT_PORT 55555

int main() {
    cout << "=============SERVER============\n";

    SOCKET serverSocket, acceptSocket;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);

    // Initialize Winsock
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        cout << "The Winsock dll not found!" << endl;
        return 0;
    }
    else {
        cout << "The Winsock dll found!" << endl;
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "socket() is OK!" << endl;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
    service.sin_port = htons(DEFAULT_PORT);

    // Bind the socket
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "bind() failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else {
        cout << "bind() is OK!" << endl;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "listen(): Error listening on socket " << WSAGetLastError() << endl;
        return 0;
    }
    else {
        cout << "listen() is OK, waiting for connections..." << endl;
    }

    // Accept a client connection
    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        cout << "accept failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }
    else {
        cout << "Accepted connection" << endl;
    }

    // Handle client requests
    handleClientRequests(acceptSocket);

    // Clean up
    closesocket(serverSocket);
    closesocket(acceptSocket);
    WSACleanup();

    return 0;
}