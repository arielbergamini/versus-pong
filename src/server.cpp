/*Initializes a TCP server that listens for two clients*/
//TODO --> store client sockaddr_in6 & start game loop once connected
//update ball position/bounce/move paddles based on input/detect paddle collisions
//detect scoring (recenter ball, score++), broadcast gamestate to both clients



#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


struct clientThread {
    SOCKET ClientSock;
    int clientId;
};

unsigned __stdcall handler(void *data) {
    clientThread *clientData = (clientThread *)data;
    SOCKET ClientSock = clientData->ClientSock;
    int clientId = clientData->clientId;

    //debugging
    //printf("Handler started for client %d --> tid %lu\n", clientId, GetCurrentThreadId());

    const int recvbuflen = 512;
    char recvbuf[recvbuflen];
    int iResult, iSendResult;

    printf("Client %d connected to handler\n", clientId);

    // receive until player shuts down connection
    do {

        iResult = recv(ClientSock, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("[Client %d] Bytes received: %d\n", clientId, iResult);

            // echo buffer back to sender
            iSendResult = send(ClientSock, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("[Client %d] Send failed w/ error: %d\n", WSAGetLastError());
                return 1;
            }
            printf("[Client %d] Bytes sent: %d\n", clientId, iSendResult);
        }
        else if (iResult == 0) {
            printf("[Client %d] Connection closed by client.\n", clientId);
        }
        else {
            printf("[Client %d] Recv failed w/ error: %d\n", clientId, WSAGetLastError());
            break;
        }

    } while (iResult > 0);

    closesocket(ClientSock);
    delete clientData;
    return 0;
}

int main() {

//------WINSOCK INITIALIZATION---------------------------
//--------------------------------------------------------
    
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
    freeaddrinfo(result);

    // set up the listening itself
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



//  ----------LISTENING & ACCEPTING-------------------
//  --------------------------------------------------

    int nextId = 1; //client labeling
    while (true) {
        sockaddr inaddr {};
        int socklen = sizeof(inaddr);

        SOCKET ClientSock = accept(ListenSock, &inaddr, &socklen);
        if (ClientSock == INVALID_SOCKET) {
            printf("Accept failed w/error: %d\n", WSAGetLastError());
            continue; //keep listening for next client
        } else {printf("Client connected at socket: %d\n", (int)ClientSock);}
      
        //thread data for newly connected client
        clientThread *cData = new clientThread;
        cData->ClientSock = ClientSock;
        cData->clientId = nextId++;

        //start thread
        uintptr_t threadHandle = _beginthreadex(NULL, 0, handler, (void *)cData, 0, NULL);
        if (threadHandle == 0) {
            printf("Failed to spawn thread, error %d\n", errno);
            closesocket(ClientSock);
            delete cData;
        } else {
            //printf("Spawned handler thread for client %d\n", cData->clientId);
            CloseHandle((HANDLE)threadHandle);
        }

    }
    // close the server socket
    closesocket(ListenSock);

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
    WSACleanup();

    return 0;
}