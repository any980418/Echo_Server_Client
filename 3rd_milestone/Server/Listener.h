#pragma once
#include "EpollCore.h"

class ServerService;

/*----------------
      Listener
-----------------*/

class Listener : public EpollObject
{
public:
    Listener() = default;
    virtual ~Listener();

public:
    bool StartAccept(ServerService* service);
    void CloseSocket();

public:
    virtual int GetSocket() override;
    virtual void Dispatch() override;

protected:
    int             _socket = -1;
    ServerService*  _service;
};