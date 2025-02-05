#include "ClientFunctions.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#define BUFFER_SIZE 512

using namespace std;
namespace fs = std::filesystem;

string clientWorkingDirectory = "E:\\Code\\FTP_WS\\ClientFolder";

void handleUserCommands(SOCKET clientSocket) {
    while (true) {
        displayMenu();
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:  // Change Working Directory
            sendCommand(clientSocket, "1");
            changeWorkingDirectory(clientSocket);
            break;
        case 2:  // List Files
            sendCommand(clientSocket, "2");
            listFiles(clientSocket);
            break;
        case 3:  // Download File
            sendCommand(clientSocket, "3");
            downloadFile(clientSocket);
            break;
        case 4:  // Upload File
            sendCommand(clientSocket, "4");
            uploadFile(clientSocket);
            break;
        case 5:  // Create File/Folder
            sendCommand(clientSocket, "5");
            createFileOrFolder(clientSocket);
            break;
        case 6:  // Remove File/Folder
            sendCommand(clientSocket, "6");
            removeFileOrFolder(clientSocket);
            break;
        case 7:  // Clean Window
            sendCommand(clientSocket, "7");
            system("cls");
            break;
        case 0:  // Shutdown
            sendCommand(clientSocket, "0");
            shutdownServer(clientSocket);
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void displayMenu() {
    cout << "\n============= Menu =============\n";
    cout << "1. Change Working Directory\n";
    cout << "2. List Files/Directories\n";
    cout << "3. Download File\n";
    cout << "4. Upload File\n";
    cout << "5. Create File/Folder\n";
    cout << "6. Remove File/Folder\n";
    cout << "7. Clear Screen\n";
    cout << "0. Shutdown Server\n";
    cout << "================================\n";
    cout << "Enter your choice: ";
}

void sendCommand(SOCKET clientSocket, const string& command) {
    send(clientSocket, command.c_str(), command.size(), 0);
}

void changeWorkingDirectory(SOCKET clientSocket) {
    cout << "Enter new working directory: ";
    string newDirectory;
    getline(cin, newDirectory);
    send(clientSocket, newDirectory.c_str(), newDirectory.size(), 0);

    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }
    cout << "Server: " << buffer << endl;
}

void listFiles(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
        cout << "Error receiving file list from server." << endl;
        return;
    }
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }
    cout << "Files/Directories:\n" << buffer;
}

void downloadFile(SOCKET clientSocket) {
    cout << "Enter file name to download: ";
    string fileName;
    getline(cin, fileName);
    send(clientSocket, fileName.c_str(), fileName.size(), 0);

    char buffer[BUFFER_SIZE] = "";
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        cout << "Error receiving file list from server." << endl;
        return;
    }
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }

    if (string(buffer) == "NOT FOUND") {
        cout << "File not found on server." << endl;
        return;
    }

    string filePath = clientWorkingDirectory + "\\" + fileName;
    ofstream file(filePath, ios::binary);
    file.write(buffer, bytesReceived);
    file.close();

    cout << "File downloaded to: " << filePath << endl;
}

void uploadFile(SOCKET clientSocket) {
    cout << "Enter file name to upload: ";
    string fileName;
    getline(cin, fileName);

    string filePath = clientWorkingDirectory + "\\" + fileName;
    if (!fs::exists(filePath)) {
        cout << "File not found in client directory." << endl;
        string notFound = "NOT FOUND";
        send(clientSocket, notFound.c_str(), notFound.size(), 0);
        return;
    }

    ifstream file(filePath, ios::binary);
    string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    send(clientSocket, fileName.c_str(), fileName.size(), 0);
    send(clientSocket, fileContent.c_str(), fileContent.size(), 0);

    char buffer[BUFFER_SIZE] = "";
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        cout << "Error receiving file list from client." << endl;
        return;
    }
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }
    cout << "Server: " << buffer << endl;
}

void createFileOrFolder(SOCKET clientSocket) {
    cout << "Enter 'f <filename>' to create file or 'd <dirname>' to create directory: ";
    string input;
    getline(cin, input);
    send(clientSocket, input.c_str(), input.size(), 0);

    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        cout << "Error receiving file list from server." << endl;
        return;
    }
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }
    cout << "Server: " << buffer << endl;
}

void removeFileOrFolder(SOCKET clientSocket) {
    cout << "Enter file/folder name to remove: ";
    string name;
    getline(cin, name);
    send(clientSocket, name.c_str(), name.size(), 0);

    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }
    cout << "Server: " << buffer << endl;
}

void shutdownServer(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        cout << "Error receiving file list from server." << endl;
        return;
    }
    if (bytesReceived >= sizeof(buffer)) {
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination within buffer's size
    }
    else {
        buffer[bytesReceived] = '\0';  // Null-terminate safely
    }
    cout << "Server: " << buffer << endl;
}