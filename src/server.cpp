/*Initializes a TCP server that listens for two clients*/
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512;
#define DEFAULT_PORT "27015"

int main(void)
{
    // initialize WSA variables
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSock = INVALID_SOCKET;
    SOCKET ClientSock = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[512];
    int recvbuflen = DEFAULT_BUFLEN;

    // initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed w/ error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // resolve server @ & port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("Getaddrinfo failed w/ error: %d\n", iResult);
        //freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // create listening socket so the server can listen for client connections
    ListenSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (iResult == SOCKET_ERROR)
    {
        printf("Socket failed w/ error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // setup TCP listening socket
    iResult = bind(ListenSock, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("Bind failed w/ error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSock);
        WSACleanup();
        return 1;
    }

    iResult = listen(ListenSock, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("Listen failed w/ error: %d\n", WSAGetLastError());
        closesocket(ListenSock);
        WSACleanup();
        return 1;
    }
    else {
        printf("Listening on port 27015...\n");
    }

    // accept a client socket
    ClientSock = accept(ListenSock, NULL, NULL);
        if (ClientSock == INVALID_SOCKET) {
            printf("Accept failed w/ error: %d\n", WSAGetLastError());
            closesocket(ListenSock);
            WSACleanup();
            return 1;
    }

    // close the server socket
    closesocket(ListenSock);

    // receive until player shuts down connection
    do {

        iResult = recv(ClientSock, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            // echo buffer back to sender
            iSendResult = send(ClientSock, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("Send failed w/ error: %d\n", WSAGetLastError());
                closesocket(ClientSock);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
        }
        else {
            printf("Recv failed w/ error: %d\n", WSAGetLastError());
            closesocket(ClientSock);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // shutdown connection when done
    iResult = shutdown(ClientSock, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("Shutdown failed w/ error: %d\n", WSAGetLastError());
        closesocket(ClientSock);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSock);
    WSACleanup();

    return 0;
}