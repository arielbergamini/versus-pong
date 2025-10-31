/*Initializes a TCP server that listens for two clients*/
//TODO --> store client sockaddr_in6 & start game loop once connected
//update ball position/bounce/move paddles based on input/detect paddle collisions
//detect scoring (recenter ball, score++), broadcast gamestate to both clients


#include <iostream>
#include <mutex>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>
#include "entities_server.h"
#include "gamestate_server.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

Ball ball(
        Vec2d(680.0 / 2.0f,  480.0 / 2.0f),
        Vec2d(BALL_SPEED, 0.0f));

Paddle paddle1(
    Vec2d(50.0f, 480.0 / 2.0f),
    Vec2d(0.0f, 0.0f));

Paddle paddle2(
    Vec2d(680.0 - 50.0f, 480.0 / 2.0f),
    Vec2d(0.0f, 0.0f));   

GameState state;
InputState input1{}, input2{};
std::mutex inputMutex;
bool bothConnected = false;

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
        //iResult = recv(ClientSock, recvbuf, recvbuflen, 0);
        InputState input{};
        int bytesReceived = recv(ClientSock, (char*)&input, sizeof(InputState), 0);
        if (bytesReceived > 0) {
           std::lock_guard<std::mutex> lock(inputMutex);
           if (clientId == 1) input1 = input;
           else if (clientId == 2) input2 = input;
        }
        else if (bytesReceived == 0) {
            printf("[Client %d] disconnected.\n", clientId);
        }
        else {
            printf("[Client %d] Recv failed w/ error: %d\n", clientId, WSAGetLastError());
            break;
        }

    } while (true);

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
    SOCKET ClientSock1 = INVALID_SOCKET;
    SOCKET ClientSock2 = INVALID_SOCKET;
    int nextId = 1; //client labeling

    while (true) {
        sockaddr inaddr {};
        int socklen = sizeof(inaddr);
        SOCKET ClientSock = accept(ListenSock, &inaddr, &socklen);

        if (ClientSock == INVALID_SOCKET) {
            printf("Accept failed w/error: %d\n", WSAGetLastError());
            continue; //keep listening for next client
        } 
        
        printf("Client connected at socket: %d\n", (int)ClientSock);

      
        if (nextId == 1) {
            ClientSock1 = ClientSock;
            printf("Assigned to player 1.\n");
            nextId++;
            printf("Waiting for player 2...\n");
        }
        else if (nextId == 2) {
            ClientSock2 = ClientSock;
            printf("Assigned to player 2.\n");
            printf("Both clients connected! Starting game!\n");
            break;
        }
    }


        //thread data for newly connected client
        /*clientThread *cData = new clientThread;
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
            CloseHandle((HANDLE)threadHandle);*/

    //GAME LOOP!
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now -lastTime;
        float dt = elapsed.count();
        lastTime = now;

        //receive input from players
        recv(ClientSock1, (char*)&input1, sizeof(InputState), 0);
        recv(ClientSock2, (char*)&input2, sizeof(InputState), 0);
    
        //and convert that input into movement directions
        float move1 = (input1.up ? -1.0f : 0.0f) + (input1.down ? 1.0f : 0.0f);
        float move2 = (input2.up ? -1.0f : 0.0f) + (input2.down ? 1.0f : 0.0f);
   
        //update objects
        paddle1.Update(move1, dt);
        paddle2.Update(move2, dt);
        ball.Update(dt);

        //collision handling
        Contact contact1 = Collision(ball, paddle1);
        if (contact1.type != CollisionType::None) ball.PaddleCollision(contact1);

        Contact contact2 = Collision(ball, paddle2);
        if (contact2.type != CollisionType::None) ball.PaddleCollision(contact2);

        //build & send game state
        state.ballPos = ball.position;
        state.paddle1Pos = paddle1.position;
        state.paddle2Pos = paddle2.position;

        send(ClientSock1, (char*)&state, sizeof(GameState), 0);
        send(ClientSock2, (char*)&state, sizeof(GameState), 0);

        //60fps
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }


    // close the server socket
    closesocket(ClientSock1);
    closesocket(ClientSock2);
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