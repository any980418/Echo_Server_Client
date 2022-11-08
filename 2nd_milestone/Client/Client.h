#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

#include "ErrorHandle.h"
#include "Packet.h"

#define PORT 8000

class Client
{
public:
    Client();
    ~Client();

    void MainLoop();

private:
    void EnterMenu();

    void SendEchoMsg();
    void SendSaveMsg();
    void SendDeleteMsg();
    void SendRequestMsgList();

    void SendPacket(unsigned int cmd, char* msg);

    void RecvMsgList();
    
private:
    int _sock;
};
