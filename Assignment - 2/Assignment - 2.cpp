#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <WS2tcpip.h>

#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3500

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET serverSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    while (true) {
        char messageBuffer[MAX_BUFFER + 1];
        std::cout << "Enter Message: ";
        std::cin.getline(messageBuffer, MAX_BUFFER);
        
        int bufferLen = static_cast<int>(strlen(messageBuffer));
        if (bufferLen == 0) break;
        
        int sendBytes = send(serverSocket, messageBuffer, bufferLen + 1, 0);
        std::cout << "Sent: " << messageBuffer << "(" << sendBytes << " bytes)" << std::endl;

        int receiveBytes = recv(serverSocket, messageBuffer, MAX_BUFFER, 0);
        std::cout << "Received: " << messageBuffer << " (" << receiveBytes << " bytes)" << std::endl;
    }
    closesocket(serverSocket);
    WSACleanup();
}
