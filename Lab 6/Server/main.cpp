#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAX_CLIENTS 10 // Максимальное количество клиентов

DWORD WINAPI ClientThread(LPVOID lpParam);
int IsUsernameUnique(char *username);

SOCKET ClientSockets[MAX_CLIENTS];                 // Массив сокетов клиентов
char ClientUsernames[MAX_CLIENTS][DEFAULT_BUFLEN]; // Массив имен клиентов
int NumClients = 0;                                // Текущее количество клиентов

int main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    while (1)
    {
        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Запрос имени пользователя
        char username[DEFAULT_BUFLEN];
        int usernameLength;
        do
        {
            send(ClientSocket, "Enter your username: ", strlen("Enter your username: ") + 1, 0);
            usernameLength = recv(ClientSocket, username, DEFAULT_BUFLEN, 0);
            username[usernameLength] = '\0'; // Удаляем символ новой строки
        } while (!IsUsernameUnique(username));

        // Добавляем сокет клиента и его имя в массивы
        if (NumClients < MAX_CLIENTS)
        {
            ClientSockets[NumClients] = ClientSocket;
            strcpy(ClientUsernames[NumClients], username);
            NumClients++;

            // Создаем поток для обслуживания клиента
            DWORD dwThreadId;
            HANDLE hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)ClientSocket, 0, &dwThreadId);
            if (hThread == NULL)
            {
                printf("CreateThread failed with error: %d\n", GetLastError());
                closesocket(ClientSocket);
                closesocket(ListenSocket);
                WSACleanup();
                return 1;
            }
            CloseHandle(hThread);
        }
        else
        {
            printf("Maximum number of clients reached. Closing connection.\n");
            closesocket(ClientSocket);
        }
    }

    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET ClientSocket = (SOCKET)lpParam;
    int iResult;

    while (1)
    {
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;

        // Receive from client
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);

            // Send to all other connected clients
            for (int i = 0; i < NumClients; ++i)
            {
                if (ClientSockets[i] != ClientSocket)
                { // Don't send to the client who sent the message
                    int iSendResult = send(ClientSockets[i], recvbuf, iResult, 0);
                    if (iSendResult == SOCKET_ERROR)
                    {
                        printf("send failed with error: %d\n", WSAGetLastError());
                        closesocket(ClientSocket);
                        return 1;
                    }
                    printf("Bytes sent to client %s: %d\n", ClientUsernames[i], iSendResult);
                }
            }
        }
        else if (iResult == 0)
        {
            printf("Connection closing...\n");
            return 0;
        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            return 1;
        }
    }

    // Close the socket
    closesocket(ClientSocket);
    return 0;
}

int IsUsernameUnique(char *username)
{
    for (int i = 0; i < NumClients; ++i)
    {
        if (strcmp(ClientUsernames[i], username) == 0)
            return 0; // Имя пользователя уже используется
    }
    return 1; // Имя пользователя уникально
}
