#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Tchar.h>
#include "ClientFunctions.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define DEFAULT_PORT 55555

int main() {
    cout << "=============CLIENT============\n";

    SOCKET clientSocket;
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

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "socket() is OK!" << endl;
    }

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(DEFAULT_PORT);

    // Connect to server
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        cout << "Client: connect() - Failed to connect " << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "Client: connect() is OK." << endl;
        cout << "Client: Can start sending and receiving data." << endl;
    }

    cout << "================================\n";
    cout << "Default Directory: E:\\Code\\FTP_WS\\ServerFolder\n";
    cout << "================================\n";

    // Handle user commands
    handleUserCommands(clientSocket);

    // Clean up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}