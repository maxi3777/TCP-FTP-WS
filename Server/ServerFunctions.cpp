#include "ServerFunctions.h"
#include <iostream>
#include <filesystem>
#include <fstream>

#define BUFFER_SIZE 512

using namespace std;
namespace fs = std::filesystem;

// Default server working directory
string currentWorkingDirectory = "E:\\Code\\FTP_WS\\ServerFolder";

void handleClientRequests(SOCKET acceptSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        // Receive client command
        int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Connection closed or error occurred." << endl;
            break;
        }

        if (bytesReceived >= sizeof(buffer)) {
            buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
        }
        else {
            buffer[bytesReceived] = '\0';  // Null-terminate safely
        }
        string command(buffer);

        if (command == "1") {  // Change Working Directory
            changeWorkingDirectory(acceptSocket);
        }
        else if (command == "2") {  // List Files
            listFiles(acceptSocket);
        }
        else if (command == "3") {  // Download File
            downloadFile(acceptSocket);
        }
        else if (command == "4") {  // Upload File
            uploadFile(acceptSocket);
        }
        else if (command == "5") {  // Create File/Folder
            createFileOrFolder(acceptSocket);
        }
        else if (command == "6") {  // Remove File/Folder
            removeFileOrFolder(acceptSocket);
        }
        else if (command == "7") {  // Clean Window (No-op for server)
            continue;
        }
        else if (command == "0") {  // Shutdown
            const char* shutdownMessage = "Server shutting down.";
            send(acceptSocket, shutdownMessage, strlen(shutdownMessage), 0);
            break;
        }
        else {
            const char* invalidCommand = "Invalid command.";
            send(acceptSocket, invalidCommand, strlen(invalidCommand), 0);
        }
    }
}

void changeWorkingDirectory(SOCKET acceptSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
        const char* errorMessage = "Error receiving data.";
        send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        return;
    }

    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Safely null-terminate if buffer is full
    }
    else {
        buffer[bytesReceived] = '\0';  // Safely null-terminate if within bounds
    }
    string newDirectory(buffer);

    if (fs::exists(newDirectory) && fs::is_directory(newDirectory)) {
        currentWorkingDirectory = newDirectory;
        const char* successMessage = "Directory changed successfully.";
        send(acceptSocket, successMessage, strlen(successMessage), 0);
    }
    else {
        const char* notFoundMessage = "NOT FOUND";
        send(acceptSocket, notFoundMessage, strlen(notFoundMessage), 0);
    }
}

void listFiles(SOCKET acceptSocket) {
    string fileList;
    for (const auto& entry : fs::directory_iterator(currentWorkingDirectory)) {
        fileList += entry.path().filename().string() + "\n";
    }
    send(acceptSocket, fileList.c_str(), fileList.size(), 0);
}

void downloadFile(SOCKET acceptSocket) {
    char buffer[BUFFER_SIZE] = "";
    int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
        const char* errorMessage = "Error receiving data.";
        send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        return;
    }

    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Safely null-terminate if buffer is full
    }
    else {
        buffer[bytesReceived] = '\0';  // Safely null-terminate if within bounds
    }
    string fileName(buffer);

    string filePath = currentWorkingDirectory + "\\" + fileName;
    if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
        ifstream file(filePath, ios::binary);
        string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        send(acceptSocket, fileContent.c_str(), fileContent.size(), 0);
    }
    else {
        const char* notFoundMessage = "NOT FOUND";
        send(acceptSocket, notFoundMessage, strlen(notFoundMessage), 0);
    }
}

void uploadFile(SOCKET acceptSocket) {
    char buffer[BUFFER_SIZE]="";
    int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
        const char* errorMessage = "Error receiving data.";
        send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        return;
    }

    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Safely null-terminate if buffer is full
    }
    else {
        buffer[bytesReceived] = '\0';  // Safely null-terminate if within bounds
    }
    string fileName(buffer);

    string filePath = currentWorkingDirectory + "\\" + fileName;
    ofstream file(filePath, ios::binary);

    recv(acceptSocket, buffer, sizeof(buffer), 0);
    file.write(buffer, sizeof(buffer));
    file.close();

    const char* successMessage = "File uploaded successfully.";
    send(acceptSocket, successMessage, strlen(successMessage), 0);
}

void createFileOrFolder(SOCKET acceptSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
        const char* errorMessage = "Error receiving data.";
        send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        return;
    }

    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Safely null-terminate if buffer is full
    }
    else {
        buffer[bytesReceived] = '\0';  // Safely null-terminate if within bounds
    }
    string input(buffer);

    if (input[0] == 'f') {
        string fileName = currentWorkingDirectory + "\\" + input.substr(2);
        ofstream file(fileName);
        if (file.is_open()) {
            file.close();
            const char* successMessage = "File created successfully.";
            send(acceptSocket, successMessage, strlen(successMessage), 0);
        }
        else {
            const char* errorMessage = "Error creating file.";
            send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        }
    }
    else if (input[0] == 'd') {
        string dirName = currentWorkingDirectory + "\\" + input.substr(2);
        if (fs::create_directory(dirName)) {
            const char* successMessage = "Directory created successfully.";
            send(acceptSocket, successMessage, strlen(successMessage), 0);
        }
        else {
            const char* errorMessage = "Error creating directory.";
            send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        }
    }
}

void removeFileOrFolder(SOCKET acceptSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
        const char* errorMessage = "Error receiving data.";
        send(acceptSocket, errorMessage, strlen(errorMessage), 0);
        return;
    }

    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Safely null-terminate if buffer is full
    }
    else {
        buffer[bytesReceived] = '\0';  // Safely null-terminate if within bounds
    }
    string name(buffer);

    string path = currentWorkingDirectory + "\\" + name;
    if (fs::exists(path)) {
        if (fs::is_directory(path)) {
            fs::remove_all(path);
            const char* successMessage = "Directory removed successfully.";
            send(acceptSocket, successMessage, strlen(successMessage), 0);
        }
        else if (fs::is_regular_file(path)) {
            fs::remove(path);
            const char* successMessage = "File removed successfully.";
            send(acceptSocket, successMessage, strlen(successMessage), 0);
        }
    }
    else {
        const char* notFoundMessage = "NOT FOUND";
        send(acceptSocket, notFoundMessage, strlen(notFoundMessage), 0);
    }
}