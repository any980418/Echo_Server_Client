#include "Client.h"

Client::Client()
{
    struct sockaddr_in  _serv_addr;

    _sock = socket(PF_INET, SOCK_STREAM, 0);
    if(_sock == -1) ErrorHandle("socket() Err!");

    char ip[] = "127.0.0.1";

    memset(&_serv_addr, 0, sizeof(_serv_addr));
    _serv_addr.sin_family = AF_INET;
    _serv_addr.sin_addr.s_addr = inet_addr(ip);
    _serv_addr.sin_port = htons(PORT);

    if(connect(_sock, (struct sockaddr*)&_serv_addr, sizeof(_serv_addr)) == -1)
        ErrorHandle("connect() Err!");
    else 
        cout << ip << ":" << PORT << " Connected..." << endl;
}

Client::~Client()
{
    close(_sock);
}

void Client::EnterMenu()
{
    int cmd = 0;

    cout << "-------------------------------------" << endl;
    cout << "1) Echo Message" << endl;
    cout << "2) Save Message" << endl;
    cout << "3) Delete Message" << endl;
    cout << "4) Show Message List" << endl;
    cout << "5) Exit" << endl;
    cout << "-------------------------------------" << endl;

    cout << "Enter Command > ";
    cin >> cmd;

    if(cin.fail()) 
    {
        cin.clear();
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    switch (cmd)
    {
    case 1:
        SendEchoMsg();
        break;
    case 2:
        SendSaveMsg();
        break;
    case 3:
        SendDeleteMsg();
        break;
    case 4:
        SendRequestMsgList();
        break;
    case 5:
        exit(0);
    }
}

void Client::MainLoop()
{
    while(true)
    {
        EnterMenu();
    }
}

void Client::SendEchoMsg()
{
    char msg[BUF_SIZE];

    while(true)
    {
        cout << "-------------------------------------" << endl;
        cout << "Input message to Echo (q to quit)." << endl;
        cout << "> ";
        cin.getline(msg, BUF_SIZE);

        if(strcmp(msg, "q") == 0)
            break;

        SendPacket(CMD_SEND_MSG, msg);

        Packet packet;
        recv(_sock, (char*)&packet, sizeof(Packet), MSG_WAITALL);
        cout << "Message from Server >> " << packet.body.data << endl;
    }
}

void Client::SendSaveMsg()
{
    char msg[BUF_SIZE];

    while(true)
    {
        cout << "-------------------------------------" << endl;
        cout << "Input message to Save (q to quit)." << endl;
        cout << "> ";
        cin.getline(msg, BUF_SIZE);

        if(strcmp(msg, "q") == 0)
            break;

        SendPacket(CMD_SAVE_MSG, msg);

        Packet packet;
        recv(_sock, (char*)&packet, sizeof(Packet), MSG_WAITALL);
        cout << packet.body.data << endl;

        SendRequestMsgList();
    }
}

void Client::SendDeleteMsg()
{
char msg[BUF_SIZE];

    while(true)
    {
        cout << "-------------------------------------" << endl;
        cout << "Input message to Delete (q to quit)." << endl;
        cout << "> ";
        cin.getline(msg, BUF_SIZE);

        if(strcmp(msg, "q") == 0)
            break;

        SendPacket(CMD_DELETE_MSG, msg);

        Packet packet;
        recv(_sock, (char*)&packet, sizeof(Packet), MSG_WAITALL);
        cout << packet.body.data << endl;

        SendRequestMsgList();
    }
}

void Client::SendRequestMsgList()
{
    SendPacket(CMD_MSG_LIST, nullptr);
    RecvMsgList();
}

void Client::SendPacket(unsigned int cmd, char* msg)
{
    Packet packet;
    packet.pHead.size = sizeof(packet);
    packet.body.cmd = cmd;
    if(msg != nullptr)
    {
        packet.body.dataLen = strlen(msg);
        strcpy(packet.body.data, msg);
    }
        
    if(send(_sock, (char*)&packet, sizeof(Packet), 0) == -1)
        ErrorHandle("send() Err!");
}

void Client::RecvMsgList()
{
    Packet packet;
    recv(_sock, (char*)&packet, sizeof(Packet), MSG_WAITALL);

    cout << "-----------------LOG-----------------" << endl;

    int count = packet.body.dataLen;
    for(int i = 0; i < count; i++)
    {
        recv(_sock, (char*)&packet, sizeof(Packet), MSG_WAITALL);
        cout << i + 1 << ". " << packet.body.data << endl;
    }

    cout << "-------------------------------------" << endl;
}