#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

#include <string>
#include <winsock2.h>

void handleClientRequests(SOCKET acceptSocket);
void changeWorkingDirectory(SOCKET acceptSocket);
void listFiles(SOCKET acceptSocket);
void downloadFile(SOCKET acceptSocket);
void uploadFile(SOCKET acceptSocket);
void createFileOrFolder(SOCKET acceptSocket);
void removeFileOrFolder(SOCKET acceptSocket);

#endif