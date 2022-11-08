#pragma once

#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/epoll.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

#include <fcntl.h>

#include "ErrorHandle.h"
#include "UserMng.h"

#define PORT 8000
#define EPOLL_BUF_SIZE 5120
#define EPOLL_SIZE 10

class EpollServer
{
public:
    EpollServer();
    ~EpollServer();

    void MainLoop();

protected:
    void SetSocket();

    void EpollCreate();
    int EpollWait();

    int ConnectClient();
    void CloseClient(const int& idx);

    void SetNonblockingMode(const int& fd);
    
protected:
    int                 _serv_sock = 0;
    struct epoll_event* _epoll_events = nullptr;
    int                 _epfd = 0;

    UserMng _userMng;
};