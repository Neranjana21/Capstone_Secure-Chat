#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "127.0.0.1" // Change this if server is on another machine
#define PORT 8080
#define BUFFER_SIZE 1024
#define XOR_KEY 0xAA

void encrypt_decrypt(char *data) {
    printf(" Original: %s\n", data);

    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= XOR_KEY;
    }

    printf("After XOR (Encrypted): ");
    for (int i = 0; data[i] != '\0'; i++) {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");

    printf("Decrypted Preview: ");
    for (int i = 0; data[i] != '\0'; i++) {
        printf("%c", data[i] ^ XOR_KEY);
    }
    printf("\n");
}

int main() {
    WSADATA wsaData;
    SOCKET client;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(client);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline

        encrypt_decrypt(buffer);  // Encrypt
        send(client, buffer, strlen(buffer), 0);

        int recvLen = recv(client, buffer, BUFFER_SIZE - 1, 0);
        if (recvLen <= 0) break;

        buffer[recvLen] = '\0';
        printf("\nReceived Encrypted Data from Server.\n");
        encrypt_decrypt(buffer);  // Decrypt
        printf("Server (decrypted): %s\n\n", buffer);
    }

    closesocket(client);
    WSACleanup();
    return 0;
}
