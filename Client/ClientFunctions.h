#ifndef CLIENTFUNCTIONS_H
#define CLIENTFUNCTIONS_H

#include <string>
#include <winsock2.h>

void handleUserCommands(SOCKET clientSocket);
void displayMenu();
void sendCommand(SOCKET clientSocket, const std::string& command);
void changeWorkingDirectory(SOCKET clientSocket);
void listFiles(SOCKET clientSocket);
void downloadFile(SOCKET clientSocket);
void uploadFile(SOCKET clientSocket);
void createFileOrFolder(SOCKET clientSocket);
void removeFileOrFolder(SOCKET clientSocket);
void shutdownServer(SOCKET clientSocket);

#endif