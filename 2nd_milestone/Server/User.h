#pragma once

#include <sys/socket.h>

#include <list>
using namespace std;

#include "CircularQueue.h"

class User
{
public:
    User(int fd) : _fd(fd) {};
    ~User() {};

    bool CopyRecvData(char* data, int len);
    void SendResponse();

    void ResponseSendMsg(Packet& packet);
    void ResponseSaveMsg(Packet& packet);
    void ResponseDeleteMsg(Packet& packet);
    void ResponseMsgList();
    
protected:
    int             _fd;
    list<string>    _log;
    CircularQueue   _queue;
};