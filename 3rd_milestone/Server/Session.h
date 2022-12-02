#pragma once
#include "EpollCore.h"

/*----------------
      Session
-----------------*/

class Session : public EpollObject
{
public:
    Session() {}
    Session(int serverSocket);
    virtual ~Session();

public:
    virtual int GetSocket() override;
    virtual void Dispatch() override;

public:
    // TEMP
    char _recvBuffer[1000];

private:
    int _socket = -1;
};