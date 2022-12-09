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
    /* 외부에서 사용 */
    bool StartAccept(ServerService* service);
    void CloseSocket();

public:
    /* 인터페이스 구현 */
    virtual int GetSocket() override;
    virtual void Dispatch() override;

protected:
    int             _socket = -1;
    ServerService*  _service;
};