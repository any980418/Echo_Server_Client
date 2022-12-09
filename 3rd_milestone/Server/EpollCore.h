#pragma once
#include <sys/epoll.h>
using namespace std;

#define EPOLL_SIZE 100

/*----------------
    EpollObject
-----------------*/

class EpollObject
{
public:
    virtual int GetSocket() = 0;
    virtual void Dispatch() = 0;
};

/*----------------
     EpollCore
-----------------*/

class EpollCore
{
public:
    EpollCore();
    ~EpollCore();

    int GetEpollFd() { return _epfd; }

    bool Register(EpollObject* epollObject, uint32_t events = EPOLLIN);
    bool Delete(int socket);
    bool Dispatch();

private:
    int                 _epfd = 0;
    struct epoll_event* _epollEvents = nullptr;
};