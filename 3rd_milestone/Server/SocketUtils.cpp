#include "SocketUtils.h"

#include <sys/socket.h>
#include <netinet/ip.h>

#include <cstring>
#include <unistd.h>
#include <fcntl.h>

/*----------------
    SocketUtils
-----------------*/

int SocketUtils::CreateSocket()
{
    return socket(PF_INET, SOCK_STREAM, 0);
}

void SocketUtils::SetNonblockingMode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

bool SocketUtils::SetReuseAddress(int socket, bool flag)
{
    return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(flag), sizeof(flag));
}

bool SocketUtils::Bind(int socket)
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    return bind(socket, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) != -1;
}

bool SocketUtils::Listen(int socket, int backlog)
{
    return listen(socket, backlog) != -1;
}

int SocketUtils::Accept(int socket)
{
    struct sockaddr_in  clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    return accept(socket, (struct sockaddr*)&clientAddr, &addrLen);
}

void SocketUtils::Close(int socket)
{
    if(socket != -1)
        close(socket);
    socket = -1;
}