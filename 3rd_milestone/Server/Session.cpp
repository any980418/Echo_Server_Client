#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

#include "ThreadManager.h"

#include <sys/socket.h>
#include <netinet/ip.h>

#include <errno.h>
#include <cstring>
#include <iostream>
using namespace std;

Session* CreateSession()
{
    return new Session();
}

/*----------------
      Session
-----------------*/

ThreadManager* Session::threadManger = new ThreadManager();

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
    
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

void Session::Connect(int serverSocket)
{
    _socket = SocketUtils::Accept(serverSocket);
    SocketUtils::SetNonblockingMode(_socket);
}

void Session::Disconnect()
{
    _service->GetEpollCore()->Delete(_socket);
    SocketUtils::Close(_socket);
    _service->ReleaseSession(this);

    cout << "Disconnected Client : " << _socket << endl;
    delete this;
}

int Session::GetSocket()
{
    return _socket;
}

void Session::Dispatch()
{
    Recv();
    threadManger->Launch([=](){
        Send();
    });
}

void Session::Recv()
{
    while(true)
    {
        char* buf = _recvBuffer.WritePos();
        int len = _recvBuffer.FreeSize();

        int numOfBytes = recv(_socket, buf, len, 0);

        if(numOfBytes == 0)
        {
            Disconnect();
            break;
        }
        else if(numOfBytes < 0)
        {
            if (errno == EAGAIN)        break;
            else if (errno == EINTR)    continue;
            else
            {
                cout << "Client [" << _socket <<  "] : Recv Error.." << endl;
                break;
            }
        }
        else
        {
            if(_recvBuffer.OnWrite(numOfBytes) == false)
            {
                cout << "Client [" << _socket << "] : OnWrite Overflow" << endl;
                break;
            }
        }
    }
}

void Session::Send()
{
    Packet packet;
    while(true)
    {
        int numOfBytes = _recvBuffer.PopPacket(&packet);
        if(numOfBytes == -1) break;

        _recvBuffer.OnRead(numOfBytes);
        switch(packet.body.cmd)
        {
        case CMD_SEND_MSG:
            ResponseSendMsg(&packet);
            break;

        case CMD_SAVE_MSG:
            ResponseSaveMsg(&packet);
            break;
            
        case CMD_DELETE_MSG:
            ResponseDeleteMsg(&packet);
            break;

        case CMD_MSG_LIST:
            ResponseMsgList();
            break;
        }
    }

    _recvBuffer.Clean();
}

void Session::ResponseSendMsg(Packet* packet)
{
    packet->body.cmd = RES_SEND_MSG;
    send(_socket, (char*)packet, sizeof(Packet), 0);
}

void Session::ResponseSaveMsg(Packet* packet)
{
    _log.push_back(packet->body.data);

    char successMsg[]   = "Success to Save Message.";
    
    packet->body.cmd = RES_SAVE_MSG;
    packet->body.dataLen = strlen(successMsg);
    strcpy(packet->body.data, successMsg);

    send(_socket, (char*)packet, sizeof(Packet), 0);
}

void Session::ResponseDeleteMsg(Packet* packet)
{
    list<string>::iterator it;
    for(it = _log.begin(); it != _log.end(); ++it)
    {   
        if(strcmp((*it).c_str(), packet->body.data) == 0)
        {
            _log.erase(it);
            break;
        }
    }

    char failMsg[]      = "Fail to Delete Message. Do Not Exist.";
    char successMsg[]   = "Success to Delete Message.";

    packet->body.cmd = RES_DELETE_MSG;
    if(it == _log.end())
    {
        packet->body.dataLen = strlen(failMsg);
        strcpy(packet->body.data, failMsg);
    }
    else
    {
        packet->body.dataLen = strlen(successMsg);
        strcpy(packet->body.data, successMsg);
    }

    send(_socket, (char*)packet, sizeof(Packet), 0);
}

void Session::ResponseMsgList()
{
    Packet packet;
    packet.pHead.size = sizeof(Packet);
    packet.body.dataLen = _log.size();
    send(_socket, (char*)&packet, sizeof(Packet), 0);

    for(auto it = _log.begin(); it != _log.end(); ++it)
    {
        packet.body.cmd = RES_MSG_LIST;
        packet.body.dataLen = (*it).length();
        strcpy(packet.body.data, (*it).c_str());

        send(_socket, (char*)&packet, sizeof(Packet), 0);
    }

    cout << "------------Log------------" << endl;
    for(auto it = _log.begin(); it != _log.end(); ++it)
    {
        cout << *it << endl;
    }
    cout << "---------------------------" << endl;
}