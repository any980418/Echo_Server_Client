#pragma once
#include "EpollCore.h"
#include "Listener.h"

#include <functional>
#include <map>
using namespace std;

class Session;

enum class ServiceType : uint8_t
{
    Server,
    Client
};

/*----------------
      Service
-----------------*/

using SessionFactory = function<Session*(int)>;

class Service
{
public:
    Service(ServiceType type, EpollCore* core, SessionFactory factory, int maxSessioinCount = 100);
    virtual ~Service();

    virtual bool    Start() = 0;
    bool            CanStart() { return _sessionFactory != nullptr; }

    virtual void    CloseService();
    void            SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

    Session*        CreateSession(int serverSocket);
    void            AddSession(Session* session);
    void            ReleaseSession(Session* session);
    int             GetCurrentSessionCount() { return _sessionCount; }
    int             GetMaxSessionCount() { return _maxSessionCount; }

public:
    ServiceType     GetServiceType() { return _type; }
    EpollCore*      GetEpollCore() { return _epollCore; }

protected:
    ServiceType     _type;
    EpollCore*      _epollCore = nullptr;

    map<int, Session*>  _sessions;
    int                 _sessionCount = 0;
    int                 _maxSessionCount = 0;
    SessionFactory      _sessionFactory;
};

/*----------------
   ClientService
-----------------*/

class ClientService : public Service
{
public:
    ClientService(EpollCore* core, SessionFactory factory, int maxSessioinCount = 100);
    virtual ~ClientService() {}

    virtual bool Start() override;
};

/*----------------
   ServerService
-----------------*/

class ServerService : public Service
{
public:
    ServerService(EpollCore* core, SessionFactory factory, int maxSessioinCount = 100);
    virtual ~ServerService() {}

    virtual bool Start() override;
    virtual void CloseService() override;

private:
    Listener* _listener = nullptr;
};