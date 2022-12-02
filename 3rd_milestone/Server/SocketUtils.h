#pragma once

#define PORT 8000

/*----------------
    SocketUtils
-----------------*/

class SocketUtils
{
public:
    static int CreateSocket();

    static void SetNonblockingMode(int fd);
    static bool SetReuseAddress(int socket, bool flag);

    static bool Bind(int socket);
    static bool Listen(int socket, int backlog = 100);
    static int  Accept(int socket);
    static void Close(int socket);
};

