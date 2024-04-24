#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

void SendMessages(SOCKET ConnectSocket);
void ReceiveMessages(SOCKET ConnectSocket);

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int iResult;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Start sending and receiving messages in separate threads
    HANDLE hSendThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendMessages, (LPVOID)ConnectSocket, 0, NULL);
    HANDLE hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveMessages, (LPVOID)ConnectSocket, 0, NULL);

    if (hSendThread == NULL || hRecvThread == NULL) {
        printf("CreateThread failed with error: %d\n", GetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Wait for threads to finish
    WaitForSingleObject(hSendThread, INFINITE);
    WaitForSingleObject(hRecvThread, INFINITE);

    // cleanup
    CloseHandle(hSendThread);
    CloseHandle(hRecvThread);
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

void SendMessages(SOCKET ConnectSocket) {
    int iResult;
    char sendbuf[DEFAULT_BUFLEN];

    do {
        printf("Enter message: ");
        fgets(sendbuf, DEFAULT_BUFLEN, stdin);

        // Send the message to server
        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            break;
        }
    } while (strcmp(sendbuf, "exit\n") != 0); // Exit loop if "exit" command is entered
}

void ReceiveMessages(SOCKET ConnectSocket) {
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    do {
        // Receive the response from the server
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Received: %s\n", recvbuf);
        } else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while (iResult > 0);
}
