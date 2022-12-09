#include "Listener.h"
#include "SocketUtils.h"
#include "Session.h"
#include "Service.h"

#include <iostream>
using namespace std;

/*----------------
      Listener
-----------------*/

Listener::~Listener()
{
    SocketUtils::Close(_socket);
}

bool Listener::StartAccept(ServerService* service)
{
    _service = service;
    if(_service == nullptr)
        return false;

    _socket = SocketUtils::CreateSocket();
    if(_socket == -1)
        return false;

    if(SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if(SocketUtils::Bind(_socket) == false)
        return false;

    if(SocketUtils::Listen(_socket) == false)
        return false;
    
    SocketUtils::SetNonblockingMode(_socket);

    // epollevents에 등록
    if(_service->GetEpollCore()->Register(this) == false)
        return false;
    
    cout << "Server Listening.." << endl;
    return true;
}

void Listener::CloseSocket()
{
    SocketUtils::Close(_socket);
}

int Listener::GetSocket()
{
    return _socket;
}

void Listener::Dispatch()
{
    // connect client
    Session* session = _service->ConnectSession(_socket);
    cout << "Connected Client : " << session->GetSocket() << endl;
}