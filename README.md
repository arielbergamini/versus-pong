## Versus Pong! -- Networked Pong using Winsock

Versus Pong utilizes a basic client-server setup for a networked two-player Pong game using Winsock on Windows. The server listens for incoming TCP connections, and the client connects and exchanges data, which is then echoed back to the client (currently a simple test message).

## Requirements

- Windows OS, Visual Studio (or other C++ compiler supporting Winsock2), Winsock2 libraries (included with Windows SDK)

## Building

When compiling, link the following libraries:
- Ws2_32.lib
- Mswsock.lib

These are already linked in server.cpp & client.cpp if you're using Visual Studio.

Build and run server first, then client in another terminal.

If you compile with Visual studio, run:
g++ server.cpp -o server.exe -lws2_32
and then execute with server.exe (or client.exe localhost)

## Troubleshooting

- ** Unable to connect to server **
    Make sure to run the server first and ensure that port 27015 is not blocked by your firewall.

- ** Permission denied **
    Try using a port above 1024 or run terminal as administrator. 