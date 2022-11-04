#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

#define PORT 8000
#define BUF_SIZE 1024

void ErrorHandle(const char* errmsg)
{
    cout << errmsg << endl;
    exit(1);
}

int main()
{
    int sock;
    struct sockaddr_in serv_addr;
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1) ErrorHandle("socket() Err!");

    char ip[] = "127.0.0.1";

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(PORT);

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        ErrorHandle("connect() Err!");
    else 
        cout << "Connected..." << endl;

    char msg[BUF_SIZE] = {};
    int strLen, recvLen, recvCnt;
    while(true)
    {
        cout << "Input message : ";
        cin.getline(msg, BUF_SIZE);

        if(strcmp(msg, "quit") == 0)
            break;

        strLen = write(sock, msg, strlen(msg));

        recvLen = 0;
        while(recvLen < strLen)
        {
            recvCnt = read(sock, &msg[recvLen], BUF_SIZE-1);
            if(recvCnt == -1) ErrorHandle("read() Err!");
            recvLen += recvCnt;
        }
        msg[recvLen] = '\0';
        cout << "Message from Server : " << msg << endl;
    }

    close(sock);
    return 0;
}
