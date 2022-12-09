#pragma once
#include "EpollCore.h"
#include "RecvBuffer.h"
#include "Packet.h"

#include <string>
#include <list>
using namespace std;

class Service;
class ThreadManager;

/*----------------
      Session
-----------------*/

class Session : public EpollObject
{
    friend class Listener;
    friend class IocpCore;
    friend class Service;

    enum
	{
		BUFFER_SIZE = sizeof(Packet)
	};

public:
    Session();
    virtual ~Session();

public:
    void    Connect(int serverSocket);
    void    Disconnect();
    void    SetService(Service* service) { _service = service; }

private:
    /* 인터페이스 구현 */
    virtual int     GetSocket() override;
    virtual void    Dispatch() override;

private:
    /* 전송 관련 */
    void Recv();
    void Send();

    void ResponseSendMsg(Packet* packet);
    void ResponseSaveMsg(Packet* packet);
    void ResponseDeleteMsg(Packet* packet);
    void ResponseMsgList();

private:
    Service*        _service;
    int             _socket = -1;

    RecvBuffer      _recvBuffer;
    list<string>    _log;

    static ThreadManager* threadManger;
};

Session* CreateSession();