#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define XOR_KEY 0xAA

void encrypt_decrypt(char *data) {
    printf(" Original: %s\n", data);

    // Encrypt/Decrypt in-place
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= XOR_KEY;
    }

    printf("After XOR (Encrypted): ");
    for (int i = 0; data[i] != '\0'; i++) {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");

    printf(" Decrypted Preview: ");
    for (int i = 0; data[i] != '\0'; i++) {
        printf("%c", data[i] ^ XOR_KEY);  // show what it would look like decrypted again
    }
    printf("\n");
}

int main() {
    WSADATA wsaData;
    SOCKET server, client;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    if (listen(server, 1) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    printf("Waiting for client...\n");
    client = accept(server, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (client == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    printf("Client connected.\n");

    while (1) {
        int recvLen = recv(client, buffer, BUFFER_SIZE - 1, 0);
        if (recvLen <= 0) break;

        buffer[recvLen] = '\0';
        printf("\nReceived Encrypted Data from Client.\n");
        encrypt_decrypt(buffer);  // decrypt in place
        printf("Client (decrypted): %s\n", buffer);

        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline

        encrypt_decrypt(buffer);  // encrypt in place
        send(client, buffer, strlen(buffer), 0);
    }

    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}
