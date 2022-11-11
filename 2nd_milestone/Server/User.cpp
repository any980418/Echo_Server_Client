#include "User.h"

bool User::CopyRecvData(char* data, int len)
{
    _queue.enqueue(data, len);
    SendResponse();
    return true;
    //return _queue.enqueue(data, len);
}

void User::SendResponse()
{
    Packet packet;
    while(_queue.isEnoughData(sizeof(Packet)) == true)
    {
        if(_queue.dequeue(&packet) == true)
        {
            switch (packet.body.cmd)
            {
            case CMD_SEND_MSG:
                ResponseSendMsg(packet);
                break;

            case CMD_SAVE_MSG:
                ResponseSaveMsg(packet);
                break;
            
            case CMD_DELETE_MSG:
                ResponseDeleteMsg(packet);
                break;

            case CMD_MSG_LIST:
                ResponseMsgList();
                break;
            }
        }
    }
}

void User::ResponseSendMsg(Packet& packet)
{
    packet.body.cmd = RES_SEND_MSG;
    send(_fd, (char*)&packet, sizeof(Packet), 0);
}

void User::ResponseSaveMsg(Packet& packet)
{
    _log.push_back(packet.body.data);

    char successMsg[]   = "Success to Save Message.";
    
    packet.body.cmd = RES_SAVE_MSG;
    packet.body.dataLen = strlen(successMsg);
    strcpy(packet.body.data, successMsg);

    send(_fd, (char*)&packet, sizeof(Packet), 0);
}

void User::ResponseDeleteMsg(Packet& packet)
{
    list<string>::iterator it;
    for(it = _log.begin(); it != _log.end(); ++it)
    {   
        if(strcmp((*it).c_str(), packet.body.data) == 0)
        {
            _log.erase(it);
            break;
        }
    }

    char failMsg[]      = "Fail to Delete Message. Do Not Exist.";
    char successMsg[]   = "Success to Delete Message.";

    packet.body.cmd = RES_DELETE_MSG;
    if(it == _log.end())
    {
        packet.body.dataLen = strlen(failMsg);
        strcpy(packet.body.data, failMsg);
    }
    else
    {
        packet.body.dataLen = strlen(successMsg);
        strcpy(packet.body.data, successMsg);
    }

    send(_fd, (char*)&packet, sizeof(Packet), 0);
}

void User::ResponseMsgList()
{
    Packet packet;
    packet.pHead.size = sizeof(Packet);
    packet.body.dataLen = _log.size();
    send(_fd, (char*)&packet, sizeof(Packet), 0);

    for(auto it = _log.begin(); it != _log.end(); ++it)
    {
        packet.body.cmd = RES_MSG_LIST;
        packet.body.dataLen = (*it).length();
        strcpy(packet.body.data, (*it).c_str());

        send(_fd, (char*)&packet, sizeof(Packet), 0);
    }

    cout << "------------Log------------" << endl;
    for(auto it = _log.begin(); it != _log.end(); ++it)
    {
        cout << *it << endl;
    }
    cout << "---------------------------" << endl;
}