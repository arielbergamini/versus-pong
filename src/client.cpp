//TODO --> Connect to server ip/port, send hello, & wait for svr welcome
//call sdl get keyboard state --> map keys --> send udp packet throttled
//render gamestate frame by frame --> listen for nonblocking gamestate packets, render using those coords
//interpolation?

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <string>

#include "gamestate.h"
#include "game.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main (int argc, char **argv) {
    WSAData wsaData;
    SOCKET ConnectSock = INVALID_SOCKET; //init socket as invalid
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;

    // validate parameters
    if (argc != 2) {
        printf("usage: %s <server-name>\n", argv[0]);
        return 1;
    }

    // initialize winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed w/ error: %d\n", iResult);
        return 1;
    }

    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    
    // resolve the server @ & port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("Getaddrinfo failed w/ error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // attempt to connect to address until success
    for(ptr=result; ptr != NULL; ptr=ptr->ai_next) {

        // create socket for connecting to the server
        ConnectSock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSock == INVALID_SOCKET) {
            printf("Socket failed w/ error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // connect to server
        iResult = connect(ConnectSock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSock);
            ConnectSock = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    } 

    printf("Connected to server! Waiting for server game state..\n");

    //loop(ConnectSock);

    // shutdown connection
    iResult = shutdown(ConnectSock, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("Shutdown failed w/ error: %d\n", WSAGetLastError());
        closesocket(ConnectSock);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSock);
    WSACleanup();

    return 0;
}

