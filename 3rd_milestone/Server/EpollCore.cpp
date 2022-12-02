#include "EpollCore.h"

#include <unistd.h>
#include <iostream>
using namespace std;

/*----------------
     EpollCore
-----------------*/

EpollCore::EpollCore()
{
    _epfd = epoll_create(EPOLL_SIZE);
    _epollEvents = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
}

EpollCore::~EpollCore()
{
    close(_epfd);
}

bool EpollCore::Register(EpollObject* epollObject, uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    event.data.ptr = reinterpret_cast<void*>(epollObject);

    if(epoll_ctl(_epfd, EPOLL_CTL_ADD, epollObject->GetSocket(), &event) == 0)
        return true;

    return false;
}

bool EpollCore::Dispatch()
{
    EpollObject* epollObject = nullptr;

    int eventCount = epoll_wait(_epfd, _epollEvents, EPOLL_SIZE, -1);

    if(eventCount == -1)
        return false;

    for(int i = 0; i < eventCount; i++)
    {
        epollObject = reinterpret_cast<EpollObject*>(_epollEvents[i].data.ptr);
        epollObject->Dispatch();
    }

    return true;
}