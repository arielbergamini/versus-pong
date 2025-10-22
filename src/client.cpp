#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <string>

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
    const char *sendbuf = "test test test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

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


    // send initial buffer
    iResult = send(ConnectSock, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("Send failed w/ error: %d\n", WSAGetLastError());
        closesocket(ConnectSock);
        WSACleanup();
        return 1;
    } else {
        printf("Bytes sent: %ld\n", iResult);
    }

    // shutdown connection
    iResult = shutdown(ConnectSock, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("Shutdown failed w/ error: %d\n", WSAGetLastError());
        closesocket(ConnectSock);
        WSACleanup();
        return 1;
    }

    // receive until server closes connection
    do {
        iResult = recv(ConnectSock, recvbuf, recvbuflen, 0);
        recvbuf[iResult] = '\0';

        if (iResult > 0) {
            printf("Message received: %s\n", recvbuf);
        } else if (iResult == 0) {
            printf("Connection closed\n");
        } else {
            printf("recv failed w/ error: %d\n", WSAGetLastError());
        }
    } while (iResult>0);

    // cleanup
    closesocket(ConnectSock);
    WSACleanup();

    return 0;
}

