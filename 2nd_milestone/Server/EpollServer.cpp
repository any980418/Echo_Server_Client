#include "EpollServer.h"

EpollServer::EpollServer()
{
    SetSocket();
    EpollCreate();
}

EpollServer::~EpollServer()
{
    close(_serv_sock);
    close(_epfd);
}

void EpollServer::SetSocket()
{
    _serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(_serv_sock == -1) ErrorHandle("socket() Err!");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if(bind(_serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
        ErrorHandle("bind() Err!");
    
    if(listen(_serv_sock, 10) == -1)
        ErrorHandle("listen() Err!");

    cout << "Server Listening.." << endl;
}

void EpollServer::SetNonblockingMode(const int& fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void EpollServer::EpollCreate()
{
    struct epoll_event  event;

    _epfd = epoll_create(EPOLL_SIZE);
    _epoll_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    SetNonblockingMode(_serv_sock);

    event.events = EPOLLIN;
    event.data.fd = _serv_sock;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, _serv_sock, &event);
}

int EpollServer::EpollWait()
{
    return epoll_wait(_epfd, _epoll_events, EPOLL_SIZE, -1);
}

int EpollServer::ConnectClient()
{
    int                 clnt_sock;
    struct sockaddr_in  clnt_addr;
    struct epoll_event  event;

    socklen_t addr_sz = sizeof(clnt_addr);
    clnt_sock = accept(_serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);

    SetNonblockingMode(clnt_sock);

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = clnt_sock;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, clnt_sock, &event);

    cout << "connected client : " << clnt_sock << endl;

    return clnt_sock;
}

void EpollServer::CloseClient(const int& idx)
{
    epoll_ctl(_epfd, EPOLL_CTL_DEL, _epoll_events[idx].data.fd, NULL);
    close(_epoll_events[idx].data.fd);

    cout << "Closed client: " << _epoll_events[idx].data.fd << endl;
}

void EpollServer::MainLoop()
{
    char buf[EPOLL_BUF_SIZE];

    while(true)
    {
        int eventCnt = EpollWait();

        if(eventCnt == -1)
        {
            cout << "epoll_wait() Err!" << endl;
            break;
        }

        for(int i = 0; i < eventCnt; i++)
        {
            if(_epoll_events[i].data.fd == _serv_sock)
            {
                int clnt_sock = ConnectClient();
                _userMng.AddUser(clnt_sock);
            }
            else
            {
                int offset = 0;

                while(true)
                {
                    int strLen = recv(_epoll_events[i].data.fd, buf + offset, BUF_SIZE, 0);
                    
                    if(strLen == 0)
                    {
                        _userMng.DeleteUser(_epoll_events[i].data.fd);
                        CloseClient(i);

                        break;
                    }
                    else if(strLen < 0)
                    {
                        if(errno == EAGAIN)
                            break;
                        else if(errno == EINTR)
                            continue;
                        else
                            cout << "recv() Err!" << endl;
                    }
                    else
                    {
                        if(offset + strLen > EPOLL_BUF_SIZE)
                        {
                            // TODO
                            break;
                        }

                        offset += strLen;
                    }
                }

                if(offset > 0)
                {
                    _userMng.FindUser(_epoll_events[i].data.fd).CopyRecvData(buf, offset);
                }
            }
        }
    }
}