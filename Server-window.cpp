#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#elif _WIN32_WINNT < 0x0600
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

void simulateScroll(int direction)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = direction; // Positive for up, negative for down
    SendInput(1, &input, sizeof(INPUT));
}

int main()
{
    // Initialize Winsock
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
    {
        std::cerr << "Can't start Winsock! Error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Can't create a socket! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 2;
    }

    // Bind the socket to an IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface

    if (bind(serverSocket, (sockaddr *)&hint, sizeof(hint)) == SOCKET_ERROR)
    {
        std::cerr << "Can't bind socket! Error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 3;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Can't listen on socket! Error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 4;
    }

    std::cout << "Waiting for a connection..." << std::endl;

    // Accept a connection
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr *)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Problem with client connection! Error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 5;
    }

    // // Print client's IP address and port
    // char clientIP[NI_MAXHOST];
    // ZeroMemory(clientIP, NI_MAXHOST);

    // inet_ntop(AF_INET, &client.sin_addr, clientIP, NI_MAXHOST);
    // std::cout << "Client connected! IP: " << clientIP << " Port: " << ntohs(client.sin_port) << std::endl;

    std::cout << "Client connected!!" << std::endl;

    // Receive and display messages
    while (true)
    {
        int messageLength = 0;

        // Receive the length of the message
        int bytesReceived = recv(clientSocket, (char *)&messageLength, sizeof(messageLength), 0);

        if (bytesReceived <= 0)
        {
            std::cout << "Client disconnected or error receiving message length." << std::endl;
            break;
        }

        // Adjust the received length (if necessary for your protocol)
        messageLength -= 3376;
        if (messageLength <= 0 || messageLength > 4096)
        {
            std::cerr << "Invalid message length received: " << messageLength << std::endl;
            continue;
        }

        std::cout << "message length: " << messageLength << std::endl;

        // Receive the actual message
        char *buffer = new char[messageLength + 1];
        ZeroMemory(buffer, messageLength + 1);

        int totalBytesReceived = 0;
        while (totalBytesReceived < messageLength)
        {
            int chunk = recv(clientSocket, buffer + totalBytesReceived, messageLength - totalBytesReceived, 0);
            if (chunk <= 0)
            {
                std::cerr << "Error receiving message data." << std::endl;
                delete[] buffer;
                closesocket(clientSocket);
                WSACleanup();
                return 6;
            }
            totalBytesReceived += chunk;
        }

        // Display the received message
        std::string message(buffer);
        std::cout << "Message received: " << message << std::endl;

        delete[] buffer;

        if (message == "exit")
        {
            std::cout << "Client requested to terminate the connection." << std::endl;
            break;
        }

        if (message == "up")
        {
            simulateScroll(120); // Scroll up
        }
        else if (message == "down")
        {
            simulateScroll(-120); // Scroll down
        }
    }

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    std::cout << "Server shut down." << std::endl;
    return 0;
}
