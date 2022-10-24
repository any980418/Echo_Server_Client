#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/epoll.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

#include <errno.h>

#define PORT 8000
#define EPOLL_SIZE 10
#define BUF_SIZE 1024

void ErrorHandle(const char* errmsg)
{
    cout << errmsg << endl;
    cout << "errno : " << errno << endl;
    exit(1);
}

int main()
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    char buf[BUF_SIZE];
    int strLen;

    struct epoll_event* ep_events;
    struct epoll_event  event;
    int epfd, event_cnt;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) ErrorHandle("socket() Err!");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
        ErrorHandle("bind() Err!");
    
    if(listen(serv_sock, 10) == -1)
        ErrorHandle("listen() Err!");

    cout << "Server Listening.." << endl;
    
    epfd = epoll_create(EPOLL_SIZE);
    ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(true)
    {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);

        if(event_cnt == -1)
        {
            cout << "wait() Err!" << endl;
            break;
        }

        for(int i = 0; i < event_cnt; i++)
        {
            if(ep_events[i].data.fd == serv_sock)
            {
                socklen_t addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);

                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);

                cout << "connected client : " << clnt_sock << endl;
            }
            else
            {
                strLen = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if(strLen == 0)
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    cout << "Closed client: " << ep_events[i].data.fd << endl;
                }
                else{
                    buf[strLen] = '\0';
                    cout << "client " << ep_events[i].data.fd << " : " << buf << endl;
                    write(ep_events[i].data.fd, buf, strLen);
                }
            }
        }
    }

    close(serv_sock);
    close(epfd);

    return 0;
}
